/*
 * TCPMain.c
 *
 *  Created on: 20 авг. 2021 г.
 *      Author: rura
 */

#include "lwip.h"
#include "TCPMain.h"
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
void tcpMainFirstString(){
	sprintf(TCPMainBuffer,"{\"deviceinfo\":{\"id\":%d,\"onboard\":{\"GPRS\":true,\"GSM\":true,\"ETH\":true,\"USB\":true},\"soft\":{\"version\":\"versionsoftware\"}},\"kye\":\"open key string\"}",8888);
}
void tcpMainNextString(){
	sprintf(TCPMainBuffer,"Ready");
}
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
		sprintf(TCPMainBuffer,"Не могу создать сокет %d",errno);
		Debug_Message(LOG_ERROR, TCPMainBuffer);
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
		Debug_Message(LOG_ERROR, "Нет соединения с сервером");
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
			Debug_Message(LOG_ERROR, "Не смог передать строку");
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

void TCPMainReadSetup(void) {
	JSON_Value *root = ShareGetJson("tcpmain");
	JSON_Object *object = json_value_get_object(root);
	TCPMainSetup.port = (int) json_object_get_number(object, "port");
	TCPMainSetup.timeoutRead = (int) json_object_get_number(object, "timeoutread");
	TCPMainSetup.timeoutWrite = (int) json_object_get_number(object, "timeoutwrite");
	TCPMainSetup.timeoutConn = (int) json_object_get_number(object, "timeoutconn");
	sscanf(json_object_get_string(object, "ip"), "%d.%d.%d.%d",
			&TCPMainSetup.adr1, &TCPMainSetup.adr2, &TCPMainSetup.adr3,
			&TCPMainSetup.adr4);
}

void TCPMainWriteSetup(void) {
	JSON_Value *root_value = json_value_init_object();
	JSON_Object *root_object = json_value_get_object(root_value);
	sprintf(TCPMainBuffer, "%d.%d.%d.%d", TCPMainSetup.adr1, TCPMainSetup.adr2,
			TCPMainSetup.adr3, TCPMainSetup.adr4);
	json_object_set_string(root_object, "ip", TCPMainBuffer);
	json_object_set_number(root_object, "port", TCPMainSetup.port);
	json_object_set_number(root_object, "timeoutread", TCPMainSetup.timeoutRead);
	json_object_set_number(root_object, "timeoutwrite", TCPMainSetup.timeoutWrite);
	json_object_set_number(root_object, "timeoutconn", TCPMainSetup.timeoutConn);
	ShareSetJson("tcpmain", root_value);
}

