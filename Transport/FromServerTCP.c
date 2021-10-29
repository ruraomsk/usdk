#include "lwip.h"
#include "DebugLogger.h"
#include "DeviceLogger.h"
#include "Transport.h"
#include "sockets.h"
#include "CommonData.h"
#include "service.h"
StatusSet ss;
char bufferFromServerTCP [ MAX_LEN_TCP_MESSAGE ];
void FromServerTCPLoop(void) {
	char name [ ] = "FromTCP";
	int sock = -1;
	bool connTcp = false;
	char *buff = NULL;
	TCPSet tcpSet;
	GetCopy(TCPSetMainName, &tcpSet);
	int err;
	struct sockaddr_in srv_addr;
	err = inet_aton(tcpSet.ip, &srv_addr.sin_addr.s_addr);
	srv_addr.sin_family = AF_INET;
	srv_addr.sin_port = htons(tcpSet.port);
	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock < 0) {
		Debug_Message(LOG_ERROR, "%s Не могу создать сокет %d", name, errno);
		BadTCP(sock);
		return;
	}
	err = connect(sock, (struct sockaddr* ) &srv_addr, sizeof(struct sockaddr_in));
	if (err < 0) {
		Debug_Message(LOG_ERROR, "%s Нет соединения с сервером", name);
		BadTCP(sock);
		return;
	}
	Debug_Message(LOG_INFO, "%s Соеденился с сервером %d", name,sock);
	buff = makeConnectString("ETH");
	strcpy(bufferFromServerTCP,buff);
	vPortFree(buff);
	err = sendString(sock, bufferFromServerTCP, strlen(bufferFromServerTCP));
	if (err < 0) {
		Debug_Message(LOG_ERROR, "%s Не смог передать строку %.60s", name, buff);
		BadTCP(sock);
		return;
	}
	setToServerTCPStart(true);
	setGoodTCP(true);
	setGPRSNeed(false);
	for (;;) {
		time_t start=GetDeviceTime();
		GetCopy(TCPSetMainName, &tcpSet);
		memset(bufferFromServerTCP, 0, sizeof(bufferFromServerTCP));
//		Debug_Message(LOG_INFO, "%s Встал на прием ", name);
		setToServerTCPStart(true);
		setGoodTCP(true);
		setGPRSNeed(false);
		readString(sock, bufferFromServerTCP, sizeof(bufferFromServerTCP), tcpSet.tque);
		int len = strlen(bufferFromServerTCP);
		if (len < 1) {
			Debug_Message(LOG_ERROR, "%s Ошибка чтения  %d", name,errno);
			if(errno!=11)	{
				BadTCP(sock);
				return;
			}
			errno=0;
			strcpy(bufferFromServerTCP,"repeat again\n");
		}
		setToServerTCPStart(true);
		setGoodTCP(true);
		setGPRSNeed(false);

		deleteEnter(bufferFromServerTCP);
		if(!MakeReplay(&connTcp, bufferFromServerTCP,name)){
			BadTCP(sock);
			return;
		}
		osDelay(STEP_CONTROL);
		setToServerTCPStart(true);
		setGoodTCP(true);
		setGPRSNeed(false);
		err = sendString(sock, bufferFromServerTCP, strlen(bufferFromServerTCP));
		if (err < 0) {
			Debug_Message(LOG_ERROR, "%s Не смог передать строку ответа %.60s", name, bufferFromServerTCP);
			BadTCP(sock);
			return;
		}
		setToServerTCPStart(true);
		setGoodTCP(true);
		setGPRSNeed(false);
		GetCopy(StatusSetName,&ss);
		ss.ethernet=true;
		ss.tobm=tcpSet.tque/60;
		ss.sServer=0;
		SetCopy(StatusSetName,&ss);
		ErrorSet es;
		GetCopy(ErrorSetName,&es);
		es.DRTC=isTimeCorrect();
		SetCopy(ErrorSetName,&es);
	}
}
