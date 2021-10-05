#include "lwip.h"
#include "DebugLogger.h"
#include "DeviceLogger.h"
#include "Transport.h"
#include "sockets.h"

#include "Files.h"
#include "CommonData.h"
extern osMessageQueueId_t GPRSToServerSecQueue;
extern osMessageQueueId_t GPRSFromServerSecQueue;

void ToServerGPRSLoop(void) {
	char name[]="ToServerFPRS";
	MessageFromQueue msg;
	int socket = -1;
	char *buffer = NULL;
	if (isGoodTCP()) return;
	TCPSet tcpSet;
	GetCopy("csec", &tcpSet);
	int err;
	struct sockaddr_in srv_addr;
	srv_addr.sin_family = AF_INET;
	srv_addr.sin_port = htons(tcpSet.port);
	inet_aton(tcpSet.ip, &srv_addr.sin_addr.s_addr);

	socket = socket(AF_INET, SOCK_STREAM, 0);
	if (socket < 0) {
		Debug_Message(LOG_ERROR, "%s Не могу создать сокет %d",name, errno);
		BadGPRS(buffer, socket, GPRSFromServerSecQueue);
		return;
	}
	//Устанавливаем тайм ауты
	struct timeval tv = { 0, 0 };
	tv.tv_sec = tcpSet.tread;
	err = lwip_setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
	tv.tv_sec = tcpSet.twrite;
	err = lwip_setsockopt(socket, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
	uint32_t toque = (uint32_t) tcpSet.tque * 1000U;
	toque = toque / STEP_CONTROL;

	err = connect(socket, (struct sockaddr* ) &srv_addr, sizeof(struct sockaddr_in));
	if (err != 0) {
		Debug_Message(LOG_ERROR, "%s Нет соединения с сервером по резервному каналу",name);
		BadGPRS(buffer, socket, GPRSFromServerSecQueue);
		return;
	}
	buffer = makeConnectString(MAX_LEN_TCP_MESSAGE, "GPRS");
	int len = strlen(buffer);
	buffer [ len ] = '\n';
	buffer [ len + 1 ] = 0;
	err = send(socket, buffer, strlen(buffer), 0);
	if (err < 0) {
		Debug_Message(LOG_ERROR, "%s Не смог передать строку %.60s",name, buffer);
		BadGPRS(buffer, socket, GPRSFromServerSecQueue);
		vPortFree(buffer);
		return;
	}
	memset(buffer, 0, MAX_LEN_TCP_MESSAGE);
	recv(socket, buffer, MAX_LEN_TCP_MESSAGE,0);
	len=strlen(buffer);
	if (len < 1) {
		Debug_Message(LOG_ERROR, "%s Ошибка чтения ",name);
		BadGPRS(buffer, socket, GPRSFromServerSecQueue);
		vPortFree(buffer);
		return;
	}
	deleteEnter(buffer);
	msg.message = buffer;
	msg.error = TRANSPORT_OK;
	Debug_Message(LOG_INFO, "%s принял %.60s",name, msg.message);
	osMessageQueuePut(GPRSFromServerSecQueue, &msg, 0, 0);
	buffer=NULL;
	for (;;) {
		GetCopy("csec", &tcpSet);
		toque = (uint32_t) tcpSet.tque * 1000U;
		toque = toque / STEP_CONTROL;
		int count = toque;
		while (osMessageQueueGet(GPRSToServerSecQueue, &msg, NULL, STEP_CONTROL/2) != osOK) {
			if (--count < 0 || !isGoodGPRS()) {
				BadGPRS(buffer, socket, GPRSFromServerSecQueue);
				return;
			}
//			if (count%10==0) Debug_Message(LOG_INFO, "%s ждем %d", name,count);
			osDelay(STEP_CONTROL/2);
		}
		int len = strlen(msg.message);
		msg.message[ len ] = '\n';
		msg.message[ len + 1 ] = 0;
		err = send(socket, msg.message, strlen(msg.message), 0);
		if (err < 0) {
			Debug_Message(LOG_ERROR, "%s Не смог передать строку %.60s",name, msg.message);
			BadGPRS(buffer, socket, GPRSFromServerSecQueue);
			vPortFree(msg.message);
			return;
		}
		Debug_Message(LOG_INFO, "%s передал %.60s",name, msg.message);
		buffer = pvPortMalloc(MAX_LEN_TCP_MESSAGE);
		if (buffer == NULL) {
			Debug_Message(LOG_ERROR, "%s Нет памяти",name);
			BadGPRS(buffer, socket, GPRSFromServerSecQueue);
			return;
		}
		memset(buffer, 0, MAX_LEN_TCP_MESSAGE);
		recv(socket, buffer, MAX_LEN_TCP_MESSAGE,0);
		len=strlen(buffer);
		if (len < 1) {
			Debug_Message(LOG_ERROR, "%s Ошибка чтения ",name);
			BadGPRS(buffer, socket, GPRSFromServerSecQueue);
			vPortFree(buffer);
			return;
		}
		deleteEnter(buffer);
		msg.message = buffer;
		msg.error = TRANSPORT_OK;
		Debug_Message(LOG_INFO, "%s принял %.60s", name,msg.message);
		osMessageQueuePut(GPRSFromServerSecQueue, &msg, 0, 0);
		buffer=NULL;
	}
}
