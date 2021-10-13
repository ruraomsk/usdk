//#include "lwip.h"
//#include "socket.h"
//#include "DebugLogger.h"
//#include "DeviceLogger.h"
//#include "Transport.h"
//#include "sockets.h"
//#include "Files.h"
//#include "CommonData.h"
//
//extern osMessageQueueId_t ETHToServerSecQueue;
//extern osMessageQueueId_t ETHFromServerSecQueue;
//char bufferToServerTCP [ MAX_LEN_TCP_MESSAGE ];
void ToServerTCPLoop(void) {
//	char name [ ] = "ToServerTCP";
//	MessageFromQueue msg;
//	char *buffer = NULL;
//	int sock = -1;
//
//	TCPSet tcpSet;
//	GetCopy(TCPSetSecName, &tcpSet);
//
//	int err;
//	struct sockaddr_in srv_addr;
//	inet_aton(tcpSet.ip, &srv_addr.sin_addr.s_addr);
//	srv_addr.sin_family = AF_INET;
//	srv_addr.sin_port = htons(tcpSet.port);
//	sock = socket(AF_INET, SOCK_STREAM, 0);
//	if (sock < 0) {
//		Debug_Message(LOG_ERROR, "%s Не могу создать сокет %d", name, errno);
//		BadTCP(sock, ETHFromServerSecQueue);
//		return;
//	}
//	//Устанавливаем тайм ауты
//	struct timeval tv = { 0, 0 };
//	tv.tv_sec = tcpSet.tread;
//	err = lwip_setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
//	tv.tv_sec = tcpSet.twrite;
//	err = lwip_setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
//	err = connect(sock, (struct sockaddr* ) &srv_addr, sizeof(struct sockaddr_in));
//	if (err != 0) {
//		Debug_Message(LOG_ERROR, "%s Нет соединения с сервером по резервному каналу", name);
//		BadTCP(sock, ETHFromServerSecQueue);
//		return;
//	}
//	buffer = makeConnectString("ETH");
//	err = sendString(sock, buffer, strlen(buffer));
//	if (err < 0) {
//		Debug_Message(LOG_ERROR, "%s Не смог передать строку %.60s", name, buffer);
//		BadTCP(sock, ETHFromServerSecQueue);
//		return;
//	}
//	vPortFree(buffer);
//	memset(bufferToServerTCP, 0, sizeof(bufferToServerTCP));
//	readString(sock, bufferToServerTCP, sizeof(bufferToServerTCP),tcpSet.tread);
//	int len = strlen(bufferToServerTCP);
//	if (len < 1) {
//		Debug_Message(LOG_ERROR, "%s Ошибка чтения ", name);
//		BadTCP(sock, ETHFromServerSecQueue);
//		return;
//	}
//	deleteEnter(bufferToServerTCP);
//	msg.message = pvPortMalloc(len);
//	if (msg.message == NULL) {
//		Debug_Message(LOG_ERROR, "%s Нет памяти %d", name, len);
//		BadTCP(sock, ETHFromServerSecQueue);
//		return;
//	}
//	msg.error = TRANSPORT_OK;
//	memcpy(msg.message, bufferToServerTCP, strlen(bufferToServerTCP)+1);
//	Debug_Message(LOG_INFO, "%s принял %.60s", name, msg.message);
//	osMessageQueuePut(ETHFromServerSecQueue, &msg, 0, 0);
//	for (;;) {
//		GetCopy(TCPSetSecName, &tcpSet);
//		Debug_Message(LOG_INFO, "%s встал на прием из очереди", name);
//		osStatus_t res = osMessageQueueGet(ETHToServerSecQueue, &msg, NULL, tcpSet.tque * 1000U);
//		if (res != osOK) {
//			Debug_Message(LOG_ERROR, "%s нет сообщения или сброс ", name);
//			BadTCP(sock, ETHFromServerSecQueue);
//		}
//		if (msg.error != TRANSPORT_OK) {
//			Debug_Message(LOG_ERROR, "%s разрыв связи ", name);
//			BadTCP(sock, ETHFromServerSecQueue);
//		}
//		err = sendString(sock, msg.message, strlen(msg.message));
//		if (err < 0) {
//			Debug_Message(LOG_ERROR, "%s Не смог передать строку ответа %.60s", name, msg.message);
//			vPortFree(msg.message);
//			BadTCP(sock, ETHFromServerSecQueue);
//			return;
//		}
//		vPortFree(msg.message);
//		Debug_Message(LOG_INFO, "%s передал %.60s", name, msg.message);
//		memset(bufferToServerTCP, 0, sizeof(bufferToServerTCP));
//		readString(sock, bufferToServerTCP, sizeof(bufferToServerTCP),tcpSet.tread);
//		len = strlen(bufferToServerTCP);
//		if (len < 1) {
//			Debug_Message(LOG_ERROR, "%s Ошибка чтения ", name);
//			BadTCP(sock, ETHFromServerSecQueue);
//			return;
//		}
//		deleteEnter(bufferToServerTCP);
//		msg.message = pvPortMalloc(len);
//		if (msg.message == NULL) {
//			Debug_Message(LOG_ERROR, "%s Нет памяти %d", name, len);
//			BadTCP(sock, ETHFromServerSecQueue);
//			return;
//		}
//		msg.error = TRANSPORT_OK;
//		memcpy(msg.message, bufferToServerTCP, strlen(bufferToServerTCP)+1);
//		osMessageQueuePut(ETHFromServerSecQueue, &msg, 0, osWaitForever);
//		Debug_Message(LOG_INFO, "%s принял %.60s", name, msg.message);
//	}
}
