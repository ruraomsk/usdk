/*
 * TCPMain.c
 *
 *  Created on: 20 авг. 2021 г.
 *      Author: rura
 */

#include "lwip.h"
#include "DebugLogger.h"
#include "DeviceLogger.h"
#include "Transport.h"
#include "sockets.h"
#include "Files.h"
#include "CommonData.h"

extern osMessageQueueId_t ToServerSecQueue;
extern osMessageQueueId_t FromServerSecQueue;

void ToServerTCPLoop(void) {
	char *buffer = NULL;
	int socket = -1;

	TCPSet tcpSet;
	GetCopy("cmain",&tcpSet);

	int err;
	struct sockaddr_in srv_addr;
	inet_aton(tcpSet.ip, &srv_addr.sin_addr.s_addr);
	srv_addr.sin_family = AF_INET;
	srv_addr.sin_port = htons(tcpSet.port);
	socket = socket(AF_INET, SOCK_STREAM, 0);
	if (socket < 0) {
		DeviceLog(SUB_TRANSPORT, "ToServerTCP Не могу создать сокет %d", errno);
		BadTCP(buffer,socket,FromServerSecQueue);
		return;
	}
	//Устанавливаем тайм ауты
	struct timeval tv = { 0, 0 };
	tv.tv_sec = tcpSet.tread;
	err = lwip_setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
	tv.tv_sec = tcpSet.twrite;
	err = lwip_setsockopt(socket, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
	uint32_t toque = (uint32_t) tcpSet.tque* 1000U;

	err = connect(socket, (struct sockaddr* ) &srv_addr,
			sizeof(struct sockaddr_in));
	if (err != 0) {
		DeviceLog(SUB_TRANSPORT,
				"ToServerTCP Нет соединения с сервером по резервному каналу");
		BadTCP(buffer,socket,FromServerSecQueue);
		return;
	}
	buffer =makeConnectString(MAX_LEN_TCP_MESSAGE, "ETH");
	int len = strlen(buffer);
	buffer[len] = '\n';
	buffer[len + 1] = 0;
	err = send(socket, buffer, strlen(buffer), 0);
	osDelay(10);
	if (err < 0) {
		DeviceLog(SUB_TRANSPORT, "ToServerTCP Не смог передать строку %.20s", buffer);
		BadTCP(buffer,socket,FromServerSecQueue);
		return;
	}
	vPortFree(buffer);
	for (;;) {
		MessageFromQueue msg;
		int count=toque;
		while (osMessageQueueGet(ToServerSecQueue, &msg, NULL, toque) != osOK) {
			if(--count<0 || !isGoodTCP()){
				DeviceLog(SUB_TRANSPORT, "ToServerTCP нет сообщения или сброс ");
				BadTCP(buffer,socket,FromServerSecQueue);
				return;
			}
		}
		vPortFree(buffer);
		int len = strlen(msg.message);
		buffer = pvPortMalloc(len + 2);
		if(buffer==NULL){
			DeviceLog(SUB_TRANSPORT, "ToServerTCP Нет памяти");
			BadTCP(buffer,socket,FromServerSecQueue);
			return;
		}
		buffer[len] = '\n';
		buffer[len + 1] = 0;
		err = send(socket, buffer, strlen(buffer), 0);
		osDelay(10);
		if (err < 0) {
			DeviceLog(SUB_TRANSPORT, "ToServerTCP Не смог передать строку %.20s", buffer);
			BadTCP(buffer,socket,FromServerSecQueue);
			return;
		}
		vPortFree(buffer);
		buffer = pvPortMalloc(MAX_LEN_TCP_MESSAGE);
		if(buffer==NULL){
			DeviceLog(SUB_TRANSPORT, "ToServerTCP Нет памяти");
			BadTCP(buffer,socket,FromServerSecQueue);
			return;
		}
		memset(buffer,0,MAX_LEN_TCP_MESSAGE);
		len = recv(socket, buffer, MAX_LEN_TCP_MESSAGE-1, 0);
		if (len < 1) {
			DeviceLog(SUB_TRANSPORT, "ToServerTCP Ошибка чтения ");
			BadTCP(buffer,socket,FromServerSecQueue);
			return;
		}
		if (buffer[len - 1] != '\n'){
			DeviceLog(SUB_TRANSPORT, "ToServerTCP Неверное завершение строки %.20s",buffer);
			BadTCP(buffer,socket,FromServerSecQueue);
			return;
		}
		buffer[len - 1] = 0;
		msg.message = buffer;
		msg.error = TRANSPORT_OK;
		osMessageQueuePut(FromServerSecQueue, &msg, 0, 0);
	}
}
