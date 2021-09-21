#include "lwip.h"
#include "DebugLogger.h"
#include "DeviceLogger.h"
#include "Transport.h"
#include "sockets.h"
#include "CommonData.h"

extern osMessageQueueId_t ToServerQueue;
extern osMessageQueueId_t FromServerQueue;
char small[64];
void FromServerTCPLoop(void) {
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
		DeviceLog(SUB_TRANSPORT, "FromServerTCP Не могу создать сокет %d", errno);
		BadTCP(buffer, socket, FromServerQueue);
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
		Debug_Message(LOG_ERROR, "FromServerTCP %s:%d %d",tcpSet.ip,tcpSet.port,errno);
		DeviceLog(SUB_TRANSPORT, "FromServerTCP Нет соединения с сервером по основному каналу");
		BadTCP(buffer, socket, FromServerQueue);
		return;
	}
	buffer = makeConnectString(MAX_LEN_TCP_MESSAGE, "ETH");
	int len = strlen(buffer);
	buffer [ len ] = '\n';
	buffer [ len + 1 ] = 0;
	err = send(socket, buffer, strlen(buffer), 0);
	if (err < 0) {
		DeviceLog(SUB_TRANSPORT, "FromServerTCP Не смог передать строку %.20s", buffer);
		BadTCP(buffer, socket, FromServerQueue);
		return;
	}
	vPortFree(buffer);
	for (;;) {
		buffer = pvPortMalloc(MAX_LEN_TCP_MESSAGE);
		if (buffer == NULL) {
			DeviceLog(SUB_TRANSPORT, "FromServerTCP Нет памяти");
			BadTCP(buffer, socket, FromServerQueue);
			return;
		}
		readSocket(socket, buffer, MAX_LEN_TCP_MESSAGE);
		if (strlen(buffer) < 1) {
			DeviceLog(SUB_TRANSPORT, "FromServerTCP Ошибка чтения ");
			BadTCP(buffer, socket, FromServerQueue);
			return;
		}
		tv.tv_sec = tcpSet.tread;
		err = lwip_setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
		setFromServerTCPStart(1);
		setGoodTCP(1);
		Debug_Message(LOG_INFO, "FromServerTCP принял %.20s", buffer);
		buffer [ len - 1 ] = 0;
		MessageFromQueue msg;
		msg.message = buffer;
		msg.error = TRANSPORT_OK;
		osMessageQueuePut(FromServerQueue, &msg, 0, 0);
		int count = toque;
		while (osMessageQueueGet(ToServerQueue, &msg, NULL, STEP_CONTROL) != osOK) {
			if (--count < 0 || !isGoodTCP()) {
				DeviceLog(SUB_TRANSPORT, "FromServerTCP Таймаут или сброс");
				BadTCP(buffer, socket, FromServerQueue);
				return;
			}
		}
		if(msg.error!=TRANSPORT_OK){
			DeviceLog(SUB_TRANSPORT, "FromServerTCP Приказ остановиться");
			BadTCP(buffer, socket, FromServerQueue);
			return;
		}
		msg.message [ len ] = '\n';
		msg.message [ len + 1 ] = 0;
		err = send(socket, msg.message, strlen(msg.message), 0);
		if (err < 0) {
			DeviceLog(SUB_TRANSPORT, "FromServerTCP Не смог передать строку ответа %.20s", buffer);
			BadTCP(buffer, socket, FromServerQueue);
			return;
		}
		Debug_Message(LOG_INFO, "FromServerTCP передал %.20s", msg.message);
		vPortFree(msg.message);
	}
}
