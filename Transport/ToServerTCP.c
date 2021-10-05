#include "lwip.h"
#include "DebugLogger.h"
#include "DeviceLogger.h"
#include "Transport.h"
#include "sockets.h"
#include "Files.h"
#include "CommonData.h"

extern osMessageQueueId_t ETHToServerSecQueue;
extern osMessageQueueId_t ETHFromServerSecQueue;
char bufferToServerTCP[MAX_LEN_TCP_MESSAGE];
void ToServerTCPLoop(void) {
	char name[]="ToServerTCP";
	MessageFromQueue msg;
	char *buffer = NULL;
	int socket = -1;

	TCPSet tcpSet;
	GetCopy("csec", &tcpSet);

	int err;
	struct sockaddr_in srv_addr;
	inet_aton(tcpSet.ip, &srv_addr.sin_addr.s_addr);
	srv_addr.sin_family = AF_INET;
	srv_addr.sin_port = htons(tcpSet.port);
	socket = socket(AF_INET, SOCK_STREAM, 0);
	if (socket < 0) {
		Debug_Message(LOG_ERROR, "%s Не могу создать сокет %d",name, errno);
		BadTCP(socket, ETHFromServerSecQueue);
		return;
	}
	//Устанавливаем тайм ауты
	struct timeval tv = { 0, 0 };
	tv.tv_sec = tcpSet.tread;
	err = lwip_setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
	tv.tv_sec = tcpSet.twrite;
	err = lwip_setsockopt(socket, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
	err = connect(socket, (struct sockaddr* ) &srv_addr, sizeof(struct sockaddr_in));
	if (err != 0) {
		Debug_Message(LOG_ERROR, "%s Нет соединения с сервером по резервному каналу",name);
		BadTCP(socket, ETHFromServerSecQueue);
		return;
	}
	buffer = makeConnectString(MAX_LEN_TCP_MESSAGE, "ETH");
	int len = strlen(buffer);
	buffer [ len ] = '\n';
	buffer [ len + 1 ] = 0;
	strncpy(bufferToServerTCP,buffer,sizeof(bufferToServerTCP));
	vPortFree(buffer);
	err = send(socket, bufferToServerTCP, strlen(bufferToServerTCP), 0);
	if (err < 0) {
		Debug_Message(LOG_ERROR, "%s Не смог передать строку %.60s",name, buffer);
		BadTCP(socket, ETHFromServerSecQueue);
		return;
	}
	memset(bufferToServerTCP, 0, MAX_LEN_TCP_MESSAGE);
	read(socket, bufferToServerTCP, MAX_LEN_TCP_MESSAGE);
	len=strlen(bufferToServerTCP);
	if (len < 1) {
		Debug_Message(LOG_ERROR, "%s Ошибка чтения ",name);
		BadTCP(socket, ETHFromServerSecQueue);
		return;
	}
	deleteEnter(bufferToServerTCP);
	msg.message = pvPortMalloc(len);
	if(msg.message==NULL){
		Debug_Message(LOG_ERROR, "%s Нет памяти %d",name,len);
		BadTCP(socket, ETHFromServerSecQueue);
		return;
	}
	msg.error = TRANSPORT_OK;
	memcpy(msg.message,bufferToServerTCP,len);
	Debug_Message(LOG_INFO, "%s принял %.60s", name,msg.message);
	osMessageQueuePut(ETHFromServerSecQueue, &msg, 0, 0);
	for (;;) {
		GetCopy("csec", &tcpSet);
		time_t start=GetDeviceTime();
		while (osMessageQueueGet(ETHToServerSecQueue, &msg, NULL, STEP_CONTROL) != osOK) {
			if (DiffTimeSecond(start)>tcpSet.tque  || !isGoodTCP()) {
				Debug_Message(LOG_ERROR, "%s нет сообщения или сброс ",name);
				BadTCP(socket, ETHFromServerSecQueue);
				return;
			}
			osDelay(STEP_CONTROL);
		}
		int len = strlen(msg.message);
		msg.message [ len ] = '\n';
		msg.message [ len + 1 ] = 0;
		strncpy(bufferToServerTCP,msg.message,strlen(msg.message));
		vPortFree(msg.message);

		err = send(socket, bufferToServerTCP, strlen(bufferToServerTCP), 0);
		if (err < 0) {
			Debug_Message(LOG_ERROR, "%s Не смог передать строку %.60s",name, bufferToServerTCP);
			BadTCP(socket, ETHFromServerSecQueue);
			return;
		}
		Debug_Message(LOG_INFO, "%s передал %.60s",name, bufferToServerTCP);
		memset(bufferToServerTCP, 0, MAX_LEN_TCP_MESSAGE);
		read(socket, bufferToServerTCP, MAX_LEN_TCP_MESSAGE);
		len=strlen(bufferToServerTCP);
		if (len < 1) {
			Debug_Message(LOG_ERROR, "%s Ошибка чтения ",name);
			BadTCP(socket, ETHFromServerSecQueue);
			return;
		}
		deleteEnter(bufferToServerTCP);
		msg.message = pvPortMalloc(len);
		if(msg.message==NULL){
			Debug_Message(LOG_ERROR, "%s Нет памяти %d",name,len);
			BadTCP(socket, ETHFromServerSecQueue);
			return;
		}
		msg.error = TRANSPORT_OK;
		memcpy(msg.message,bufferToServerTCP,len);
		osMessageQueuePut(ETHFromServerSecQueue, &msg, 0, 0);
		Debug_Message(LOG_INFO, "%s принял %.60s",name, msg.message);
	}
}
