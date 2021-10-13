//#include "lwip.h"
//#include "socket.h"
//#include "DebugLogger.h"
//#include "DeviceLogger.h"
//#include "Transport.h"
//#include "sockets.h"
//#include "Files.h"
//#include "CommonData.h"
//
//extern osMessageQueueId_t GPRSToServerSecQueue;
//extern osMessageQueueId_t GPRSFromServerSecQueue;
//char bufferToServerGPRS[MAX_LEN_TCP_MESSAGE];
void ToServerGPRSLoop(void) {
//	if(isGoodTCP()) return;
//	if(!isGoodGPRS()) return;
//	char name[]="ToServerGPRS";
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
//		Debug_Message(LOG_ERROR, "%s Не могу создать сокет %d",name, errno);
//		BadTCP(sock, GPRSFromServerSecQueue);
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
//		Debug_Message(LOG_ERROR, "%s Нет соединения с сервером по резервному каналу",name);
//		BadTCP(sock, GPRSFromServerSecQueue);
//		return;
//	}
//	buffer = makeConnectString("GPRS");
//	err = sendString(sock, buffer, strlen(buffer));
//	if (err < 0) {
//		Debug_Message(LOG_ERROR, "%s Не смог передать строку %.60s",name, buffer);
//		BadTCP(sock, GPRSFromServerSecQueue);
//		return;
//	}
//	vPortFree(buffer);
//	memset(bufferToServerGPRS, 0, MAX_LEN_TCP_MESSAGE);
//	read(sock, bufferToServerGPRS, MAX_LEN_TCP_MESSAGE);
//	int len=strlen(bufferToServerGPRS);
//	if (len < 1) {
//		Debug_Message(LOG_ERROR, "%s Ошибка чтения ",name);
//		BadTCP(sock, GPRSFromServerSecQueue);
//		return;
//	}
//	deleteEnter(bufferToServerGPRS);
//	msg.message = pvPortMalloc(len);
//	if(msg.message==NULL){
//		Debug_Message(LOG_ERROR, "%s Нет памяти %d",name,len);
//		BadTCP(sock, GPRSFromServerSecQueue);
//		return;
//	}
//	msg.error = TRANSPORT_OK;
//	memcpy(msg.message,bufferToServerGPRS,len);
//	Debug_Message(LOG_INFO, "%s принял %.60s", name,msg.message);
//	osMessageQueuePut(GPRSFromServerSecQueue, &msg, 0, 0);
//	for (;;) {
//		GetCopy(TCPSetSecName, &tcpSet);
//		time_t start=GetDeviceTime();
//		while (true) {
//			osDelay(STEP_CONTROL/2);
//			if(osMessageQueueGetCount(GPRSToServerSecQueue)!= 0) break;
//			if (DiffTimeSecond(start) > tcpSet.tque || !isGoodGPRS()|| isGoodTCP()) {
//				Debug_Message(LOG_ERROR, "%s нет сообщения или сброс ",name);
//				BadTCP(sock, GPRSFromServerSecQueue);
//				return;
//			}
//		}
//		osMessageQueueGet(GPRSToServerSecQueue, &msg, NULL, osWaitForever);
//		err = sendString(sock, msg.message, strlen(msg.message));
//		if (err < 0) {
//			Debug_Message(LOG_ERROR, "%s Не смог передать строку ответа %.60s", name, msg.message);
//			vPortFree(msg.message);
//			BadTCP(sock, GPRSFromServerSecQueue);
//			return;
//		}
//		vPortFree(msg.message);
//		Debug_Message(LOG_INFO, "%s передал %.60s", name, msg.message);
//		memset(bufferToServerGPRS, 0, MAX_LEN_TCP_MESSAGE);
//		read(sock, bufferToServerGPRS, MAX_LEN_TCP_MESSAGE);
//		len=strlen(bufferToServerGPRS);
//		if (len < 1) {
//			Debug_Message(LOG_ERROR, "%s Ошибка чтения ",name);
//			BadTCP(sock, GPRSFromServerSecQueue);
//			return;
//		}
//		deleteEnter(bufferToServerGPRS);
//		msg.message = pvPortMalloc(len);
//		if(msg.message==NULL){
//			Debug_Message(LOG_ERROR, "%s Нет памяти %d",name,len);
//			BadTCP(sock, GPRSFromServerSecQueue);
//			return;
//		}
//		msg.error = TRANSPORT_OK;
//		memcpy(msg.message,bufferToServerGPRS,len);
//		osMessageQueuePut(GPRSFromServerSecQueue, &msg, 0, 0);
//		Debug_Message(LOG_INFO, "%s принял %.60s",name, msg.message);
//	}
}
