/*
 * TCPMain.c
 *
 *  Created on: 20 авг. 2021 г.
 *      Author: rura
 */

#include "../EthernetTCP/TCPMain.h"

#include "lwip.h"
#include "DebugLogger.h"
#include "parson.h"
#include "sockets.h"
#include "share.h"

struct {
	int adr1, adr2, adr3, adr4;
	unsigned int port;
	unsigned int timeoutRead;
	unsigned int timeoutWrite;
	unsigned int timeoutConn;
} TCPMainSetup;
char TCPMainBuffer[MAX_LEN_TCP_MESSAGE];
char *ptrMainBuffer = TCPMainBuffer;

int ReadyMainConnect = 0;

void TCPMainLoop(void) {
	TCPMainReadSetup();
	int err;
	struct sockaddr_in srv_addr;
	sprintf(TCPMainBuffer, "%d.%d.%d.%d", TCPMainSetup.adr1, TCPMainSetup.adr2,
			TCPMainSetup.adr3, TCPMainSetup.adr4);
	inet_aton(TCPMainBuffer, &srv_addr.sin_addr.s_addr);
	srv_addr.sin_family = AF_INET;
	srv_addr.sin_port = htons(TCPMainSetup.port);
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0) {
		Debug_Message(LOG_ERROR, "Не могу создать сокет %d",errno);
		return;
	}
	//Устанавливаем тайм ауты
	struct timeval tv={0,0};
	tv.tv_sec=TCPMainSetup.timeoutRead;
	err=lwip_setsockopt(sock,SOL_SOCKET,SO_RCVTIMEO,&tv,sizeof(tv));
	tv.tv_sec=TCPMainSetup.timeoutWrite;
	err=lwip_setsockopt(sock,SOL_SOCKET,SO_SNDTIMEO,&tv,sizeof(tv));
//	tv.tv_sec=TCPMainSetup.timeoutConn;
//	err=lwip_setsockopt(sock,SOL_SOCKET,SO_CONTIMEO,&tv,sizeof(tv));

	err = connect(sock, (struct sockaddr*) &srv_addr,
			sizeof(struct sockaddr_in));
	if (err != 0) {
		Debug_Message(LOG_ERROR, "Нет соединения с сервером %s:%d errno %d socket:%d",TCPMainBuffer,TCPMainSetup.port,errno,sock);
		shutdown(sock, 0);
		close(sock);
		return;
	}

	tcpMainFirstString();
	strcat(TCPMainBuffer,"\n\r");
	err = send(sock, TCPMainBuffer, strlen(TCPMainBuffer), 0);
	if (err < 0) {
		Debug_Message(LOG_ERROR, "Не смог передать стартовую строку");
		shutdown(sock, 0);
		close(sock);
		return;
	}

	while (1) {
		int len = recv(sock, TCPMainBuffer, sizeof(TCPMainBuffer) - 1, 0);
		// Error occurred during receiving
		if (len < 0) {
			Debug_Message(LOG_ERROR, "Ошибка чтения ");
			shutdown(sock, 0);
			close(sock);
			break;
		}
		// Data received
		else {
			TCPMainBuffer[len] = 0; // Null-terminate whatever we received and treat like a string
		}
		Debug_Message(LOG_INFO, TCPMainBuffer);
		tcpMainNextString();
		strcat(TCPMainBuffer,"\n\r");
		int err = send(sock, TCPMainBuffer, strlen(TCPMainBuffer), 0);
		if (err < 0) {
			Debug_Message(LOG_ERROR, "Не смог передать строку %s",TCPMainBuffer);
			shutdown(sock, 0);
			close(sock);
			return;
		}

		osDelay(100);
	}

	if (sock != -1) {
		Debug_Message(LOG_ERROR, "Разорвано соединение с сервером");
		shutdown(sock, 0);
		close(sock);
	}

}

