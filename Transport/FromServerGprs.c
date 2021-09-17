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
#include "sockets.h"
#include "Files.h"
#include "CommonData.h"

extern osMessageQueueId_t GPRSToServerQueue;
extern osMessageQueueId_t GPRSFromServerQueue;

void FromServerGPRSLoop(void) {
	int socket = -1;
	char *buffer = NULL;
	if (isGoodTCP()) return;
	int err;
	struct sockaddr_in srv_addr;
	TCPSet tcpSet;
	GetCopy("csec",&tcpSet);
	inet_aton(tcpSet.ip, &srv_addr.sin_addr.s_addr);
	srv_addr.sin_family = AF_INET;
	srv_addr.sin_port = htons(tcpSet.port);
	socket = socket(AF_INET, SOCK_STREAM, 0);
	if (socket < 0) {
		DeviceLog(SUB_TRANSPORT, "FromServerGPRS Не могу создать сокет %d", errno);
		BadGPRS(buffer, socket, GPRSFromServerQueue);
		return;
	}
	//Устанавливаем тайм ауты
	struct timeval tv = { 0, 0 };
	tv.tv_sec = tcpSet.tread;
	err = lwip_setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
	tv.tv_sec = tcpSet.twrite;
	err = lwip_setsockopt(socket, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
	uint32_t toque = (uint32_t) tcpSet.tque* 1000U;
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
	buffer =makeConnectString(MAX_LEN_TCP_MESSAGE, "GPRS");
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
