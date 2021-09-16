/*
 * FromServerGprs.c
 *
 *  Created on: 30 авг. 2021 г.
 *      Author: rura
 */

#include "lwip.h"
#include "DebugLogger.h"
#include "DeviceLogger.h"
#include "Transport.h"
#include "parson.h"
#include "sockets.h"
#include "Files.h"

extern osMessageQueueId_t GPRSToServerQueue;
extern osMessageQueueId_t GPRSFromServerQueue;

void FromServerGPRSLoop(void) {
	int socket = -1;
	char *buffer = NULL;
	if (isGoodTCP()) return;
	DeviceStatus deviceStatus = readSetup("setup");
	if (deviceStatus.ID<0){
		BadGPRS(buffer, socket, GPRSFromServerQueue);
		Debug_Message(LOG_FATAL, "GPRS Нет настроек setup");
		return;
	}
	if (!deviceStatus.Gprs) {
		DeviceLog(SUB_TRANSPORT, "FromServer Нет GPRS");
		BadGPRS(buffer, socket, GPRSFromServerQueue);
		return;
	}
	int err;
	struct sockaddr_in srv_addr;
	JSON_Value *root = FilesGetJson("cmain");
	if (root == NULL) {
		Debug_Message(LOG_FATAL, "GPRS Нет настроек");
		BadGPRS(buffer, socket, GPRSFromServerQueue);
		return;
	}
	JSON_Object *object = json_value_get_object(root);
	inet_aton(json_object_get_string(object, "ip"), &srv_addr.sin_addr.s_addr);
	srv_addr.sin_family = AF_INET;
	srv_addr.sin_port = htons((int ) json_object_get_number(object, "port"));
	socket = socket(AF_INET, SOCK_STREAM, 0);
	if (socket < 0) {
		DeviceLog(SUB_TRANSPORT, "FromServerGPRS Не могу создать сокет %d", errno);
		BadGPRS(buffer, socket, GPRSFromServerQueue);
		json_value_free(root);
		return;
	}
	//Устанавливаем тайм ауты
	struct timeval tv = { 0, 0 };
	tv.tv_sec = (int) json_object_get_number(object, "timeoutread");
	err = lwip_setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
	tv.tv_sec = (int) json_object_get_number(object, "timeoutwrite");
	err = lwip_setsockopt(socket, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
	uint32_t toque = (uint32_t) json_object_get_number(object, "timeoutque") * 1000U;
	json_value_free(root);
	toque = toque / STEP_CONTROL;
	err = connect(socket, (struct sockaddr* ) &srv_addr, sizeof(struct sockaddr_in));
	if (err != 0) {
		DeviceLog(SUB_TRANSPORT, "FromServerGPRS Нет соединения с сервером по основному каналу");
		BadGPRS(buffer, socket, GPRSFromServerQueue);
		return;
	}
	buffer = pvPortMalloc(MAX_LEN_TCP_MESSAGE);
	if(buffer==NULL){
		DeviceLog(SUB_TRANSPORT, "FromServerGPRS Нет памяти");
		BadGPRS(buffer, socket, GPRSFromServerQueue);
		return;
	}
	makeConnectString(buffer, MAX_LEN_TCP_MESSAGE, "GPRS", &deviceStatus);
	int len = strlen(buffer);
	buffer[len] = '\n';
	buffer[len + 1] = 0;
	err = send(socket, buffer, strlen(buffer), 0);
	osDelay(100);
	if (err < 0) {
		DeviceLog(SUB_TRANSPORT, "FromServerGPRS Не смог передать строку %.20s", buffer);
		BadGPRS(buffer, socket, GPRSFromServerQueue);
		return;
	}
	vPortFree(buffer);
	for (;;) {
		buffer = pvPortMalloc(MAX_LEN_TCP_MESSAGE);
		if(buffer==NULL){
			DeviceLog(SUB_TRANSPORT, "FromServerGPRS Нет памяти");
			BadGPRS(buffer, socket, GPRSFromServerQueue);
			return;
		}
		memset(buffer, 0, MAX_LEN_TCP_MESSAGE);
		len = recv(socket, buffer, MAX_LEN_TCP_MESSAGE-1, 0);
		if (len < 1) {
			DeviceLog(SUB_TRANSPORT, "FromServerGPRS Ошибка чтения ");
			BadGPRS(buffer, socket, GPRSFromServerQueue);
			return;
		}
		if (buffer[len - 1] != '\n') {
			DeviceLog(SUB_TRANSPORT, "FromServerGPRS Неверное завершение строки %.20s", buffer);
			BadGPRS(buffer, socket, GPRSFromServerQueue);
			return;
		}
		setGoodGPRS(1);
		setFromServerGPRSStart(1);
		buffer[len - 1] = 0;
		MessageFromQueue msg;
		msg.message = buffer;
		msg.error = TRANSPORT_OK;
		osMessageQueuePut(GPRSFromServerQueue, &msg, 0, 0);
		int count = toque;
		while (osMessageQueueGet(GPRSToServerQueue, &msg, NULL, STEP_CONTROL) != osOK) {
			if (--count < 0 || !isGoodGPRS() || isGoodTCP()) {
				DeviceLog(SUB_TRANSPORT, "FromServerGPRS Таймаут или сброс");
				BadGPRS(buffer, socket, GPRSFromServerQueue);
				return;
			}
		}
		if (msg.error == TRANSPORT_STOP) {
			DeviceLog(SUB_TRANSPORT, "FromServerGPRS Приказали остановиться");
			BadGPRS(buffer, socket, GPRSFromServerQueue);
			return;
		}
		int len = strlen(msg.message);
		buffer = pvPortMalloc(len + 2);
		if(buffer==NULL){
			DeviceLog(SUB_TRANSPORT, "FromServerGPRS Нет памяти");
			BadGPRS(buffer, socket, GPRSFromServerQueue);
			return;
		}
		memcpy(buffer,msg.message,len);
		vPortFree(msg.message);
		buffer[len] = '\n';
		buffer[len + 1] = 0;
		err = send(socket, buffer, strlen(buffer), 0);
		if (err < 0) {
			DeviceLog(SUB_TRANSPORT, "FromServerGPRS Не смог передать строку ответа %.20s", buffer);
			BadGPRS(buffer, socket, GPRSFromServerQueue);
			return;
		}
		vPortFree(buffer);
		osDelay(100);
	}
}
