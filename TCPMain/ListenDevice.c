
#include "ListenDevice.h"
#include "lwip.h"
#include "TCPMain.h"
#include "DebugLogger.h"
#include "parson.h"
#include "sockets.h"
#include "share.h"

char ListenBuffer[256];
void TCPListenDeviceLoop(void) {
	int port=1093;
	int err;
	struct sockaddr_in addr;
	addr.sin_addr.s_addr=htonl(INADDR_ANY);
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0) {
		Debug_Message(LOG_ERROR, "Не могу создать сокет %d",errno);
		return;
	}
	if(bind(sock,(struct sockaddr *)&addr,sizeof(addr))==-1){
		Debug_Message(LOG_ERROR, "Не могу назначить сокет");
		return;
	}
	err = listen(sock,5);
	if (err != 0) {
		Debug_Message(LOG_ERROR, "Не могу прослушивать сокет по порту %d errno=%d",port,errno);
		close(sock);
		return;
	}
	Debug_Message(LOG_INFO, "ListenDevice ready");
	for(;;){
		struct sockaddr_in client;
		int new_socket;
		new_socket=accept(sock,(struct sockaddr *)&client,sizeof(client));
		if(new_socket<0) break;
		Debug_Message(LOG_INFO, "Новый клиент %s",inet_ntoa(client.sin_addr));
		for(;;){
			int count=0;
			int len = recv(new_socket, ListenBuffer, sizeof(ListenBuffer) - 1, 0);
			if(len<0){
				Debug_Message(LOG_ERROR, "Ошибка чтения ");
				break;
			}
			Debug_Message(LOG_ERROR, "Пришло:%s",ListenBuffer);
			osDelay(1000);
			sprintf(ListenBuffer,"Sending %d",count++);
			int err = send(new_socket, ListenBuffer, strlen(ListenBuffer), 0);
			if (err < 0) {
				Debug_Message(LOG_ERROR, "Не смог передать строку %s",ListenBuffer);
				break;
			}
		}
		close(new_socket);
	}
}

