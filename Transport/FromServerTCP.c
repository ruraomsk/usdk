#include "lwip.h"
#include "DebugLogger.h"
#include "DeviceLogger.h"
#include "Transport.h"
#include "sockets.h"
#include "CommonData.h"

extern osMessageQueueId_t ETHToServerQueue;
extern osMessageQueueId_t ETHFromServerQueue;
void FromServerTCPLoop(void) {
	char name[]="FromServerTCP";
	int socket = -1;
	char *buffer = NULL;
	TCPSet tcpSet;
	GetCopy("cmain", &tcpSet);
	int err;
	struct sockaddr_in srv_addr;
	err = inet_aton(tcpSet.ip, &srv_addr.sin_addr.s_addr);
	srv_addr.sin_family = AF_INET;
	srv_addr.sin_port = htons(tcpSet.port);
	socket = socket(AF_INET, SOCK_STREAM, 0);
	if (socket < 0) {
		Debug_Message(LOG_ERROR,"%s Не могу создать сокет %d",name, errno);
		BadTCP(buffer, socket, ETHFromServerQueue);
		return;
	}
	//Устанавливаем тайм ауты
	struct timeval tv = { 0, 0 };
	tv.tv_sec = tcpSet.twrite;
	err = lwip_setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
	tv.tv_sec = tcpSet.twrite;
	err = lwip_setsockopt(socket, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
	uint32_t toque = (uint32_t) tcpSet.tque * 1000U;
	toque = toque / STEP_CONTROL;
	err = connect(socket, (struct sockaddr* ) &srv_addr, sizeof(struct sockaddr_in));
	if (err != 0) {
		Debug_Message(LOG_ERROR, "%s Нет соединения с сервером по основному каналу",name);
		BadTCP(buffer, socket, ETHFromServerQueue);
		return;
	}
	buffer = makeConnectString(MAX_LEN_TCP_MESSAGE, "ETH");
	int len = strlen(buffer);
	buffer [ len ] = '\n';
	buffer [ len + 1 ] = 0;
	err = send(socket, buffer, strlen(buffer), 0);
	if (err < 0) {
		Debug_Message(LOG_ERROR, "%s Не смог передать строку %.20s",name, buffer);
		BadTCP(buffer, socket, ETHFromServerQueue);
		return;
	}
	vPortFree(buffer);

	for (;;) {
		buffer = pvPortMalloc(MAX_LEN_TCP_MESSAGE);
		if (buffer == NULL) {
			Debug_Message(LOG_ERROR, "%s Нет памяти",name);
			BadTCP(buffer, socket, ETHFromServerQueue);
			return;
		}
		GetCopy("cmain", &tcpSet);
		tv.tv_sec = tcpSet.tread;
		err = lwip_setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
		memset(buffer,0,MAX_LEN_TCP_MESSAGE);
//		readSocket(socket, buffer, MAX_LEN_TCP_MESSAGE);
		recv(socket, buffer, MAX_LEN_TCP_MESSAGE,0);
		len=strlen(buffer);
		if (len < 1) {
			Debug_Message(LOG_ERROR, "%s Ошибка чтения ",name);
			BadTCP(buffer, socket, ETHFromServerQueue);
			vPortFree(buffer);
			return;
		}
		setToServerTCPStart(true);
		setGoodTCP(true);
		Debug_Message(LOG_INFO, "%s принял %.20s",name, buffer);
		buffer [ len - 1 ] = 0;
		MessageFromQueue msg;
		msg.message = buffer;
		msg.error = TRANSPORT_OK;
		osMessageQueuePut(ETHFromServerQueue, &msg, 0, 0);
		int count = toque;
		while (osMessageQueueGet(ETHToServerQueue, &msg, NULL, STEP_CONTROL) != osOK) {
			if (--count < 0 || !isGoodTCP()) {
				Debug_Message(LOG_ERROR, "%s Таймаут или сброс",name);
				BadTCP(buffer, socket, ETHFromServerQueue);
				return;
			}
		}
		len=strlen(msg.message);
		msg.message [ len ] = '\n';
		msg.message [ len + 1 ] = 0;
		err = send(socket, msg.message, strlen(msg.message), 0);
		if (err < 0) {
			Debug_Message(LOG_ERROR, "%s Не смог передать строку ответа %.20s",name,msg.message);
			BadTCP(buffer, socket, ETHFromServerQueue);
			vPortFree(msg.message);
			return;
		}
		Debug_Message(LOG_INFO, "%s передал %.20s", name,msg.message);
		vPortFree(msg.message);
	}
}
