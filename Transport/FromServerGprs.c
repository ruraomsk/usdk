//#include "lwip.h"
//#include "DebugLogger.h"
//#include "DeviceLogger.h"
//#include "Transport.h"
//#include "sockets.h"
//#include "CommonData.h"
//
//extern osMessageQueueId_t GPRSToServerQueue;
//extern osMessageQueueId_t GPRSFromServerQueue;
//char bufferFromServerGPRS [ MAX_LEN_TCP_MESSAGE ];
void FromServerGPRSLoop(void) {
//	if(isGoodTCP()) return;
//	char name [ ] = "FromServerGPRS";
//	int sock;
//	sock = -1;
//	char *buff = NULL;
//	TCPSet tcpSet;
//	GetCopy(TCPSetMainName, &tcpSet);
//	int err;
//	struct sockaddr_in srv_addr;
//	err = inet_aton(tcpSet.ip, &srv_addr.sin_addr.s_addr);
//	srv_addr.sin_family = AF_INET;
//	srv_addr.sin_port = htons(tcpSet.port);
//	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
//	if (sock < 0) {
//		Debug_Message(LOG_ERROR, "%s Не могу создать сокет %d", name, errno);
//		BadTCP(sock, GPRSFromServerQueue);
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
//		Debug_Message(LOG_ERROR, "%s Нет соединения с сервером по основному каналу", name);
//		BadTCP(sock, GPRSFromServerQueue);
//		return;
//	}
//	buff = makeConnectString("GPRS");
//	err = sendString(sock, buff, strlen(buff));
//	if (err < 0) {
//		Debug_Message(LOG_ERROR, "%s Не смог передать строку %.60s", name, buff);
//		BadTCP(sock, GPRSFromServerQueue);
//		return;
//	}
//	for (;;) {
//		GetCopy(TCPSetMainName, &tcpSet);
//		tv.tv_sec = tcpSet.tread;
//		err = lwip_setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
//		memset(bufferFromServerGPRS, 0, MAX_LEN_TCP_MESSAGE);
//		Debug_Message(LOG_INFO, "%s Встал на прием ", name);
//		read(sock, bufferFromServerGPRS, MAX_LEN_TCP_MESSAGE);
//		time_t startCycle=GetDeviceTime();
//		int len = strlen(bufferFromServerGPRS);
//		if (len < 1) {
//			Debug_Message(LOG_ERROR, "%s Ошибка чтения ", name);
//			BadTCP(sock, GPRSFromServerQueue);
//			return;
//		}
//		deleteEnter(bufferFromServerGPRS);
//		setToServerGPRSStart(true);
//		setGoodGPRS(true);
//		Debug_Message(LOG_INFO, "%s принял %.60s", name, bufferFromServerGPRS);
//		MessageFromQueue msg;
//		msg.message = pvPortMalloc(len);
//		if (msg.message == NULL) {
//			Debug_Message(LOG_ERROR, "%s Нет памяти %d", name, len);
//			BadTCP(sock, GPRSFromServerQueue);
//			return;
//		}
//		memcpy(msg.message, bufferFromServerGPRS, len);
//		msg.error = TRANSPORT_OK;
//		osMessageQueuePut(GPRSFromServerQueue, &msg, 0, 0);
//		GetCopy(TCPSetMainName, &tcpSet);
//		time_t start = GetDeviceTime();
//		while (true) {
//			osDelay(STEP_CONTROL/2);
//			if(osMessageQueueGetCount(GPRSToServerQueue)!=0) break;
//			if (DiffTimeSecond(start) > tcpSet.tque || !isGoodGPRS()|| isGoodTCP()) {
//				Debug_Message(LOG_ERROR, "%s Таймаут или сброс", name);
//				BadTCP(sock, GPRSFromServerQueue);
//				return;
//			}
//		}
//		osMessageQueueGet(GPRSToServerQueue, &msg, NULL, osWaitForever);
//		Debug_Message(LOG_INFO, "%s получил для передачи %.60s", name, msg.message);
//		err = sendString(sock, msg.message, strlen(msg.message));
//		if (err < 0) {
//			Debug_Message(LOG_ERROR, "%s Не смог передать строку ответа %.60s", name, msg.message);
//			vPortFree(msg.message);
//			BadTCP(sock, GPRSFromServerQueue);
//			return;
//		}
//		Debug_Message(LOG_INFO, "%s %d передал %.60s", name,DiffTimeSecond(startCycle), msg.message);
//		vPortFree(msg.message);
//	}
}
