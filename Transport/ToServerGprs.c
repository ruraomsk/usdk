/*
 * ToServerGprs.c
 *
 *  Created on: 30 авг. 2021 г.
 *      Author: rura
 */




#include "lwip.h"
#include "DebugLogger.h"
#include "Transport.h"
#include "parson.h"
#include "sockets.h"

#include "Files.h"

extern osMessageQueueId_t GPRSToServerSecQueue;
extern osMessageQueueId_t GPRSFromServerSecQueue;

void ToServerGPRSLoop(void) {
	int socket = -1;
	char *buffer = NULL;
	if (isGoodTCP()) return;
	DeviceStatus deviceStatus = readSetup("setup");
	if (!deviceStatus.Gprs) {
		Debug_Message(LOG_ERROR, "ToServer Нет GPRS");
		BadGPRS(buffer,socket,GPRSFromServerSecQueue);
		return;
	}
	int err;
	struct sockaddr_in srv_addr;
	JSON_Value *root = ShareGetJson("csec");
	if (root==NULL){
		printf("ERROR");
	}
	JSON_Object *object = json_value_get_object(root);
	inet_aton(json_object_get_string(object, "ip"), &srv_addr.sin_addr.s_addr);
	srv_addr.sin_family = AF_INET;
	srv_addr.sin_port = htons((int ) json_object_get_number(object, "port"));
	socket = socket(AF_INET, SOCK_STREAM, 0);
	if (socket < 0) {
		Debug_Message(LOG_ERROR, "ToServerGPRS Не могу создать сокет %d", errno);
		BadGPRS(buffer,socket,GPRSFromServerSecQueue);
		return;
	}
	//Устанавливаем тайм ауты
	struct timeval tv = { 0, 0 };
	tv.tv_sec = (int ) json_object_get_number(object, "timeoutread");
	err = lwip_setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
	tv.tv_sec = (int ) json_object_get_number(object, "timeoutwrite");
	err = lwip_setsockopt(socket, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
	uint32_t toque=(uint32_t) json_object_get_number(object, "timeoutque")*1000U;
	toque=toque/STEP_CONTROL;

	err = connect(socket, (struct sockaddr* ) &srv_addr,
			sizeof(struct sockaddr_in));
	if (err != 0) {
		Debug_Message(LOG_ERROR,
				"ToServerGPRS Нет соединения с сервером по резервному каналу");
		BadGPRS(buffer,socket,GPRSFromServerSecQueue);
		return;
	}
	buffer = malloc(MAX_LEN_TCP_MESSAGE);
	makeConnectString(buffer, MAX_LEN_TCP_MESSAGE, "GPRS", &deviceStatus);
	int len = strlen(buffer);
	buffer[len] = '\n';
	buffer[len + 1] = 0;
	err = send(socket, buffer, strlen(buffer), 0);
	osDelay(100);
	if (err < 0) {
		Debug_Message(LOG_ERROR, "ToServerGPRS Не смог передать строку %s", buffer);
		BadGPRS(buffer,socket,GPRSFromServerSecQueue);
		return;
	}
	for (;;) {
		MessageFromQueue msg;
		int count=toque;
		while (osMessageQueueGet(GPRSToServerSecQueue, &msg, NULL, toque) != osOK) {
			if (--count<0 || !isGoodGPRS()||isGoodTCP()){
			BadGPRS(buffer,socket,GPRSFromServerSecQueue);
			return;
			}
		}
		if(msg.error==TRANSPORT_STOP){
			Debug_Message(LOG_ERROR, "ToServerGPRS Приказали остановиться");
			BadGPRS(buffer,socket,GPRSFromServerSecQueue);
			return;
		}
		free(buffer);
		int len = strlen(msg.message);
		buffer = malloc(len + 2);
		buffer[len] = '\n';
		buffer[len + 1] = 0;
		err = send(socket, buffer, strlen(buffer), 0);
		osDelay(100);
		if (err < 0) {
			Debug_Message(LOG_ERROR, "ToServerGPRS Не смог передать строку %s", buffer);
			BadGPRS(buffer,socket,GPRSFromServerSecQueue);
			return;
		}
		free(buffer);
		buffer = malloc(MAX_LEN_TCP_MESSAGE);
		memset(buffer,0,MAX_LEN_TCP_MESSAGE);
		len = recv(socket, buffer, MAX_LEN_TCP_MESSAGE-1, 0);
		if (len < 1) {
			Debug_Message(LOG_ERROR, "ToServerGPRS Ошибка чтения ");
			BadGPRS(buffer,socket,GPRSFromServerSecQueue);
			return;
		}
		if (buffer[len - 1] != '\n'){
			Debug_Message(LOG_ERROR, "ToServerGPRS Неверное завершение строки %s",buffer);
			BadGPRS(buffer,socket,GPRSFromServerSecQueue);
			return;
		}
		buffer[len - 1] = 0;
		msg.message = buffer;
		msg.error = TRANSPORT_OK;
		osMessageQueuePut(GPRSFromServerSecQueue, &msg, 0, 0);
	}
}
