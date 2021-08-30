/*
 * TCPMain.c
 *
 *  Created on: 20 авг. 2021 г.
 *      Author: rura
 */

#include "lwip.h"
#include "DebugLogger.h"
#include "Transport.h"
#include "parson.h"
#include "sockets.h"
#include "share.h"

extern osMessageQueueId_t ToServerSecQueue;
extern osMessageQueueId_t FromServerSecQueue;

void veryBad(char *buffer,int socket,osMessageQueueId_t que) {
	free(buffer);
	MessageFromQueue msg;
	msg.error = TRANSPORT_ERROR;
	msg.message = NULL;
	osMessageQueuePut(que, &msg, 0, 0);
	shutdown(socket, 0);
	close(socket);
}
void ToServerTCPLoop(void) {
	char *buffer = NULL;
	int socket = -1;
	DeviceStatus deviceStatus = readSetup("setup");
	if (!deviceStatus.Ethertnet) {
		Debug_Message(LOG_ERROR, "Нет Ethernet");
		MessageFromQueue msg;
		msg.error = TRANSPORT_ERROR;
		msg.message = NULL;
		osMessageQueuePut(FromServerSecQueue, &msg, 0, 0);
		return;
	}
	int err;
	struct sockaddr_in srv_addr;
	JSON_Value *root = ShareGetJson("tcpconnectsecond");
	JSON_Object *object = json_value_get_object(root);
	inet_aton(json_object_get_string(object, "ip"), &srv_addr.sin_addr.s_addr);
	srv_addr.sin_family = AF_INET;
	srv_addr.sin_port = htons((int ) json_object_get_number(object, "port"));
	socket = socket(AF_INET, SOCK_STREAM, 0);
	if (socket < 0) {
		Debug_Message(LOG_ERROR, "Не могу создать сокет %d", errno);
		MessageFromQueue msg;
		msg.error = TRANSPORT_ERROR;
		msg.message = NULL;
		osMessageQueuePut(FromServerSecQueue, &msg, 0, 0);
		return;
	}
	//Устанавливаем тайм ауты
	struct timeval tv = { 0, 0 };
	tv.tv_sec = (int ) json_object_get_number(object, "timeoutread");
	err = lwip_setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
	tv.tv_sec = (int ) json_object_get_number(object, "timeoutwrite");
	err = lwip_setsockopt(socket, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
	int toque=(int ) json_object_get_number(object, "timeoutque");

	err = connect(socket, (struct sockaddr* ) &srv_addr,
			sizeof(struct sockaddr_in));
	if (err != 0) {
		Debug_Message(LOG_ERROR,
				"Нет соединения с сервером по резервному каналу");
		veryBad(buffer,socket,FromServerSecQueue);
		return;
	}
	buffer = malloc(MAX_LEN_TCP_MESSAGE);
	makeConnectString(buffer, MAX_LEN_TCP_MESSAGE, "ETH", &deviceStatus);
	int len = strlen(buffer);
	buffer[len] = '\n';
	buffer[len + 1] = 0;
	err = send(socket, buffer, strlen(buffer), 0);
	osDelay(10);
	if (err < 0) {
		Debug_Message(LOG_ERROR, "Не смог передать строку %s", buffer);
		veryBad(buffer,socket,FromServerSecQueue);
		return;
	}
	for (;;) {
		MessageFromQueue msg;

		if (osMessageQueueGet(ToServerSecQueue, &msg, NULL, toque) != osOK) {
			veryBad(buffer,socket,FromServerSecQueue);
			return;
		}
		free(buffer);
		int len = strlen(msg.message);
		buffer = malloc(len + 2);
		buffer[len] = '\n';
		buffer[len + 1] = 0;
		err = send(socket, buffer, strlen(buffer), 0);
		osDelay(10);
		if (err < 0) {
			Debug_Message(LOG_ERROR, "Не смог передать строку %s", buffer);
			veryBad(buffer,socket,FromServerSecQueue);
			return;
		}
		free(buffer);
		buffer = malloc(MAX_LEN_TCP_MESSAGE);
		memset(buffer,0,MAX_LEN_TCP_MESSAGE);
		len = recv(socket, buffer, MAX_LEN_TCP_MESSAGE-1, 0);
		if (len < 0) {
			Debug_Message(LOG_ERROR, "Ошибка чтения ");
			veryBad(buffer,socket,FromServerSecQueue);
			return;
		}
		if (len == 0) continue;
		if (buffer[len - 1] != '\n'){
			Debug_Message(LOG_ERROR, "Неверное завершение строки %s",buffer);
			veryBad(buffer,socket,FromServerSecQueue);
			return;
		}
		buffer[len - 1] = 0;
		msg.message = buffer;
		msg.error = TRANSPORT_OK;
		osMessageQueuePut(FromServerSecQueue, &msg, 0, 0);
	}
}
