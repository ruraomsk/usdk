
#include "lwip.h"
#include "DebugLogger.h"
#include "Device.h"
#include "parson.h"
#include "sockets.h"
#include "share.h"

extern osMessageQueueId_t ToServerQueue;
extern osMessageQueueId_t FromServerQueue;
static int sock = -1;
static char *buffer = NULL;
void veryBad() {
	free(buffer);
	MessageFromQueue msg;
	msg.error = true;
	msg.message = NULL;
	osMessageQueuePut(FromServerQueue, msg, 0, 0);
	shutdown(sock, 0);
	close(sock);
}
void ToServerTCPLoop() {
	DeviceStatus deviceStatus = readSetup("setupdevice");
	if (!deviceStatus.Ethertnet)
		return;

	int err;
	struct sockaddr_in srv_addr;
	JSON_Value *root = ShareGetJson("tcpconnect");
	JSON_Object *object = json_value_get_object(root);
	inet_aton(json_object_get_string(object, "ip"), &srv_addr.sin_addr.s_addr);
	srv_addr.sin_family = AF_INET;
	srv_addr.sin_port = htons((int ) json_object_get_number(object, "port"));
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0) {
		Debug_Message(LOG_ERROR, "Не могу создать сокет %d", errno);
		MessageFromQueue msg;
		msg.error = true;
		msg.message = NULL;
		osMessageQueuePut(FromServerQueue, msg, 0, 0);
		return;
	}
	//Устанавливаем тайм ауты
	struct timeval tv = { 0, 0 };
	tv.tv_sec = TCPMainSetup.timeoutRead;
	err = lwip_setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
	tv.tv_sec = TCPMainSetup.timeoutWrite;
	err = lwip_setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
//	tv.tv_sec=TCPMainSetup.timeoutConn;
//	err=lwip_setsockopt(sock,SOL_SOCKET,SO_CONTIMEO,&tv,sizeof(tv));
	err = connect(sock, (struct sockaddr* ) &srv_addr,
			sizeof(struct sockaddr_in));
	if (err != 0) {
		Debug_Message(LOG_ERROR,
				"Нет соединения с сервером по основному каналу");
		veryBad();
		return;
	}
	buffer = malloc(MAX_LEN_TCP_MESSAGE);
	makeConnectString(buffer, MAX_LEN_TCP_MESSAGE, "ETH", &deviceStatus);
	int len = strlen(buffer);
	buffer[len] = '\n';
	buffer[len + 1] = 0;
	err = send(sock, buffer, strlen(buffer), 0);
	osDelay(10);
	if (err < 0) {
		Debug_Message(LOG_ERROR, "Не смог передать строку %s", buffer);
		veryBad();
		return;
	}
	for (;;) {
		len = recv(socket, buffer, MAX_LEN_TCP_MESSAGE-1, 0);
		if (len < 0) {
			Debug_Message(LOG_ERROR, "Ошибка чтения ");
			veryBad();
			return;
		}
		if (len == 0)
			continue;
		buffer[len - 1] = 0;
		MessageFromQueue msg;
		msg.message = buffer;
		msg.error = false;
		osMessageQueuePut(FromServerQueue, &msg, 0, 0);
		if (osMessageQueueGet(ToServerQueue, &msg, NULL, 0) != osOK) {
			veryBad();
			return;
		}
		int len = strlen(msg.message);
		buffer = malloc(len + 2);

		buffer[len] = '\n';
		buffer[len + 1] = 0;
		err = send(sock, buffer, strlen(buffer), 0);
		osDelay(10);
		if (err < 0) {
			Debug_Message(LOG_ERROR, "Не смог передать строку %s", buffer);
			veryBad();
			return;
		}
	}
}
