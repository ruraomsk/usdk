#include "lwip.h"
#include "socket.h"
#include "DebugLogger.h"
#include "DeviceLogger.h"
#include "DeviceTime.h"
#include "Transport.h"
#include "sockets.h"
#include "Files.h"
#include "CommonData.h"
extern osMessageQueueId_t toServerMessageHandle;
char bufferToServerTCP [ MAX_LEN_TCP_MESSAGE ];
void ToServerTCPLoop(void) {
	char name [ ] = "ToSTCP";
	char *buffer = NULL;
	int sock = -1;

	TCPSet tcpSet;
	GetCopy(TCPSetSecName, &tcpSet);

	int err;
	struct sockaddr_in srv_addr;
	inet_aton(tcpSet.ip, &srv_addr.sin_addr.s_addr);
	srv_addr.sin_family = AF_INET;
	srv_addr.sin_port = htons(tcpSet.port);
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0) {
		Debug_Message(LOG_ERROR, "%s Не могу создать сокет %d", name, errno);
		BadTCP(sock);
		return;
	}
	err = connect(sock, (struct sockaddr* ) &srv_addr, sizeof(struct sockaddr_in));
	if (err != 0) {
		Debug_Message(LOG_ERROR, "%s Нет соединения с сервером по резервному каналу", name);
		BadTCP(sock);
		return;
	}
	Debug_Message(LOG_INFO, "%s Соеденился с сервером %d", name,sock);
	buffer = makeConnectString("ETH");
	strcpy(bufferToServerTCP,buffer	);
	vPortFree(buffer);
	err = sendString(sock, bufferToServerTCP, strlen(bufferToServerTCP));
	if (err < 0) {
		Debug_Message(LOG_ERROR, "%s Не смог передать строку %.60s", name, buffer);
		BadTCP(sock);
		return;
	}
	memset(bufferToServerTCP, 0, sizeof(bufferToServerTCP));
	readString(sock, bufferToServerTCP, sizeof(bufferToServerTCP), tcpSet.tread);
	int len = strlen(bufferToServerTCP);
	if (len < 1) {
		Debug_Message(LOG_ERROR, "%s Ошибка чтения ", name);
		BadTCP(sock);
		return;
	}
	deleteEnter(bufferToServerTCP);
	if (!isConnect(bufferToServerTCP)) {
		Debug_Message(LOG_ERROR, "%s ошибка подключения", name);
		BadTCP(sock);
		return;
	}
	MessageConfirm(bufferToServerTCP);
	err = sendString(sock, bufferToServerTCP, strlen(bufferToServerTCP));
	if (err < 0) {
		Debug_Message(LOG_ERROR, "%s Не смог передать строку %.60s", name, bufferToServerTCP);
		BadTCP(sock);
		return;
	}
	CallBackParam cp;
	cp.Signal = 1;
	cp.QueueId = toServerMessageHandle;
	osTimerId_t keepAlive = osTimerNew(CallbackQueue, osTimerOnce, &cp, NULL);
	osTimerStart(keepAlive, tcpSet.tque * 1000U);
	for (;;) {
		GetCopy(TCPSetSecName, &tcpSet);
		uint16_t msg;
//		Debug_Message(LOG_INFO, "%s встал на прием из очереди", name);
		if (!isGoodTCP()) {
			Debug_Message(LOG_ERROR, "%s отключился главный ", name);
			BadTCP(sock);
			osTimerStop(keepAlive);
			return;
		}
		osStatus_t res = osMessageQueueGet(toServerMessageHandle, &msg, NULL, osWaitForever);
		if (res != osOK) {
			Debug_Message(LOG_ERROR, "%s нет сообщения или сброс ", name);
			BadTCP(sock);
		}
		if (!isGoodTCP()) {
			Debug_Message(LOG_ERROR, "%s отключился главный ", name);
			BadTCP(sock);
			osTimerStop(keepAlive);
			return;
		}
		switch (msg) {
		case 1:
			MessageStatusDevice(bufferToServerTCP);
			break;
		default:
			break;
		}
		err = sendString(sock, bufferToServerTCP, strlen(bufferToServerTCP));
		if (err < 0) {
			Debug_Message(LOG_ERROR, "%s Не смог передать строку ответа %.60s", name, bufferToServerTCP);
			BadTCP(sock);
			osTimerStop(keepAlive);
			return;
		}
		GetCopy(TCPSetSecName, &tcpSet);
		osTimerStart(keepAlive, tcpSet.tque * 1000U);
//		Debug_Message(LOG_INFO, "%s передал %.60s", name, bufferToServerTCP);
	}
}
