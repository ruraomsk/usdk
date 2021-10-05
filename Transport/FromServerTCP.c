#include "lwip.h"
#include "DebugLogger.h"
#include "DeviceLogger.h"
#include "Transport.h"
#include "sockets.h"
#include "CommonData.h"

extern osMessageQueueId_t ETHToServerQueue;
extern osMessageQueueId_t ETHFromServerQueue;
char bufferFromServerTCP[MAX_LEN_TCP_MESSAGE];
int socketFromServerTCP;
void StopFromServerTCP(void* arg){
	Debug_Message(LOG_ERROR,"%s Таймаут %d","FromServerTCP", errno);
	BadTCP(socketFromServerTCP, ETHFromServerQueue);
}
void FromServerTCPLoop(void) {
	char name[]="FromServerTCP";
	socketFromServerTCP = -1;
	char *buff= NULL;
	TCPSet tcpSet;
	GetCopy("cmain", &tcpSet);
	int err;
	struct sockaddr_in srv_addr;
	err = inet_aton(tcpSet.ip, &srv_addr.sin_addr.s_addr);
	srv_addr.sin_family = AF_INET;
	srv_addr.sin_port = htons(tcpSet.port);
	socketFromServerTCP = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (socketFromServerTCP < 0) {
		Debug_Message(LOG_ERROR,"%s Не могу создать сокет %d",name, errno);
		BadTCP(socketFromServerTCP, ETHFromServerQueue);
		return;
	}
	//Устанавливаем тайм ауты
	struct timeval tv = { 0, 0 };
	tv.tv_sec = tcpSet.twrite;
	err = lwip_setsockopt(socketFromServerTCP, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
	tv.tv_sec = tcpSet.twrite;
	err = lwip_setsockopt(socketFromServerTCP, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
	err = connect(socketFromServerTCP, (struct sockaddr* ) &srv_addr, sizeof(struct sockaddr_in));
	if (err != 0) {
		Debug_Message(LOG_ERROR, "%s Нет соединения с сервером по основному каналу",name);
		BadTCP(socketFromServerTCP, ETHFromServerQueue);
		return;
	}
	buff = makeConnectString(MAX_LEN_TCP_MESSAGE, "ETH");
	int len = strlen(buff);
	buff [ len ] = '\n';
	buff [ len + 1 ] = 0;
	vPortFree(buff);
	strncpy(bufferFromServerTCP,buff,sizeof(bufferFromServerTCP));
	err = send(socketFromServerTCP, bufferFromServerTCP, strlen(bufferFromServerTCP), 0);
	if (err < 0) {
		Debug_Message(LOG_ERROR, "%s Не смог передать строку %.60s",name, buff);
		BadTCP(socketFromServerTCP, ETHFromServerQueue);
		return;
	}
	osTimerId_t TimeOutFromServerTCP;
	TimeOutFromServerTCP=osTimerNew(StopFromServerTCP, osTimerOnce, NULL, NULL);
	for (;;) {
		GetCopy("cmain", &tcpSet);
		tv.tv_sec = tcpSet.tread;
		err = lwip_setsockopt(socketFromServerTCP, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
		memset(bufferFromServerTCP,0,MAX_LEN_TCP_MESSAGE);
		osTimerStart(TimeOutFromServerTCP, tcpSet.tread*1000U);
		read(socketFromServerTCP, bufferFromServerTCP, MAX_LEN_TCP_MESSAGE);
		osTimerStop(TimeOutFromServerTCP);
		len=strlen(bufferFromServerTCP);
		if (len < 1) {
			Debug_Message(LOG_ERROR, "%s Ошибка чтения ",name);
			BadTCP(socketFromServerTCP, ETHFromServerQueue);
			return;
		}
		deleteEnter(bufferFromServerTCP);
		setToServerTCPStart(true);
		setGoodTCP(true);
		Debug_Message(LOG_INFO, "%s принял %.60s",name, bufferFromServerTCP);
		MessageFromQueue msg;
		msg.message = pvPortMalloc(len);
		if(msg.message==NULL){
			Debug_Message(LOG_ERROR, "%s Нет памяти %d",name,len);
			BadTCP(socketFromServerTCP, ETHFromServerQueue);
			return;
		}
		memcpy(msg.message,bufferFromServerTCP,len);
		msg.error = TRANSPORT_OK;
		osMessageQueuePut(ETHFromServerQueue, &msg, 0, 0);
		GetCopy("cmain", &tcpSet);
		time_t start=GetDeviceTime();
		while (osMessageQueueGet(ETHToServerQueue, &msg, NULL, STEP_CONTROL) != osOK) {
			if (DiffTimeSecond(start)>tcpSet.tque  || !isGoodTCP()) {
				Debug_Message(LOG_ERROR, "%s Таймаут или сброс",name);
				BadTCP(socketFromServerTCP, ETHFromServerQueue);
				return;
			}
			osDelay(STEP_CONTROL);
		}
		len=strlen(msg.message);
		msg.message [ len ] = '\n';
		msg.message [ len + 1 ] = 0;
		strncpy(bufferFromServerTCP,msg.message,sizeof(bufferFromServerTCP));
		vPortFree(msg.message);
		err = send(socketFromServerTCP, bufferFromServerTCP, strlen(bufferFromServerTCP), 0);
		if (err < 0) {
			Debug_Message(LOG_ERROR, "%s Не смог передать строку ответа %.60s",name,bufferFromServerTCP);
			BadTCP(socketFromServerTCP, ETHFromServerQueue);
			return;
		}
		Debug_Message(LOG_INFO, "%s передал %.60s", name,bufferFromServerTCP);
	}
}
