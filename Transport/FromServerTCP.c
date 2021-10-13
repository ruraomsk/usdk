#include "lwip.h"
#include "DebugLogger.h"
#include "DeviceLogger.h"
#include "Transport.h"
#include "sockets.h"
#include "CommonData.h"
#include "service.h"

char bufferFromServerTCP [ MAX_LEN_TCP_MESSAGE ];
void FromServerTCPLoop(void) {
	char name [ ] = "FromTCP";
	int sock;
	sock = -1;
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
	if (err != 0) {
		Debug_Message(LOG_ERROR, "%s Нет соединения с сервером", name);
		BadTCP(sock);
		return;
	}

	buff = makeConnectString("ETH");
	err = sendString(sock, buff, strlen(buff));
	if (err < 0) {
		Debug_Message(LOG_ERROR, "%s Не смог передать строку %.60s", name, buff);
		BadTCP(sock);
		return;
	}
	setToServerTCPStart(true);
	setGoodTCP(true);
	setGPRSNeed(false);
	for (;;) {
		GetCopy(TCPSetMainName, &tcpSet);
		memset(bufferFromServerTCP, 0, sizeof(bufferFromServerTCP));
		Debug_Message(LOG_INFO, "%s Встал на прием ", name);
		readString(sock, bufferFromServerTCP, sizeof(bufferFromServerTCP), tcpSet.tque);
		time_t startCycle = GetDeviceTime();
		int len = strlen(bufferFromServerTCP);
		if (len < 1) {
			Debug_Message(LOG_ERROR, "%s Ошибка чтения ", name);
			BadTCP(sock);
			return;
		}
		deleteEnter(bufferFromServerTCP);
		Debug_Message(LOG_INFO, "%s принял %.60s", name, bufferFromServerTCP);
		while (true) {
			if (!connTcp && isConnect(bufferFromServerTCP)) {
				prepareConnectMessage(bufferFromServerTCP);
				MessageConfirm(bufferFromServerTCP);
				connTcp = true;
				break;
			}
			if (!connTcp && !isConnect(bufferFromServerTCP)) {
				Debug_Message(LOG_ERROR, "%s ошибка подключения", name);
				BadTCP(sock);
				return;
			}
			if (isGive_Me_Status(bufferFromServerTCP)) {
				prepareGiveMeStatus(bufferFromServerTCP);
				MessageStatusDevice(bufferFromServerTCP);
				break;
			}
			char* message;
			message = doGiveCommand(bufferFromServerTCP);
			if (message == NULL) {
				Debug_Message(LOG_ERROR, "%s Неизвестная команда", name);
			}
			break;
		}
		Debug_Message(LOG_INFO, "%s получил %d для передачи %.60s", name, strlen(bufferFromServerTCP), bufferFromServerTCP);
		err = sendString(sock, bufferFromServerTCP, strlen(bufferFromServerTCP));
		if (err < 0) {
			Debug_Message(LOG_ERROR, "%s Не смог передать строку ответа %.60s", name, bufferFromServerTCP);
			BadTCP(sock);
			return;
		}
		Debug_Message(LOG_INFO, "%s %d передал %.60s", name, DiffTimeSecond(startCycle), bufferFromServerTCP);
	}
}
