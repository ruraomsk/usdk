/*
 * Camera.c
 *
 *  Created on: Sep 2, 2021
 *      Author: rura
 */
#include "lwip.h"
#include "sockets.h"
#include <stdlib.h>
#include "DebugLogger.h"
#include "Camera.h"
#include "CommonData.h"
#include "DeviceTime.h"

osTimerId_t TickerCamera=NULL;
osMessageQueueId_t CameraTickerQueue=NULL;
osMessageQueueId_t CameraStatSignalQueue=NULL;
CamerasOcup zo;
char bufferCamera [ 120 ];
int idCamera;
CallBackParam Cameraparam={.Signal=CAMERA_READ_ZONE};
bool SendToCamera(int sock,char* buffer,int len){
	if (send(sock, buffer, len, 0) < 0) {
		Debug_Message(LOG_ERROR, "Camera%d Ошибка записи ", idCamera);
		shutdown(sock, SHUT_RDWR);
		close(sock);
		osDelay(1000U);
		return false;
	}
	return true;
}
void CameraWork(void *arg) {
	char endln [ ] = "\r\n";
	char gs [ ] = "gs\r\n";
	char gdlast[]="gd last\r\n";
	int tout=10;
	CameraSet cs;
	GetCopy(CameraSetName, &cs);
	int sock = -1;
	CameraTickerQueue=osMessageQueueNew(6, sizeof(uint32_t), NULL);
	Cameraparam.QueueId=CameraTickerQueue;
	TickerCamera=osTimerNew(CallbackQueue, osTimerPeriodic, &Cameraparam, NULL);
	osTimerStart(TickerCamera, 1000U);
	for (;;) {
		time_t start;
		int command=0;
		vTaskDelay(CAMERA_STEP_CONTROL);
		osMessageQueueGet(CameraTickerQueue,&command,NULL,tout);
		if(command==0) continue;
//		Debug_Message(LOG_INFO, "Camera начинаем цикл");
		start=GetDeviceTime();
		for (int p = 0; p < MAX_CAMERAS; ++p) {
			if (cs.cameras [ p ].id != 0) {
				osDelay(20);
				int id = cs.cameras [ p ].id;
				idCamera=id;
				struct sockaddr_in srv_addr;
				inet_aton(cs.cameras [ p ].ip, &srv_addr.sin_addr.s_addr);
				srv_addr.sin_family = AF_INET;
				srv_addr.sin_port = htons(cs.cameras [ p ].port);
				sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
				if (sock < 0) {
					Debug_Message(LOG_ERROR, "Camera%d Не могу создать сокет %d", id, errno);
					osDelay(1000U);
					continue;
				}
				struct timeval tv = { 0, 0 };
				tv.tv_sec = 1;
				lwip_setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
				tv.tv_sec = 1;
				lwip_setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));

				int err = connect(sock, (struct sockaddr* ) &srv_addr, sizeof(struct sockaddr_in));
				if (err != 0) {
					Debug_Message(LOG_ERROR, "Camera%d Нет соединения ", id);
					shutdown(sock, SHUT_RDWR);
					close(sock);
					osDelay(1000U);
					continue;
				}
				if (!SendToCamera(sock, cs.cameras [ p ].login, strlen(cs.cameras [ p ].login))) {
					continue;
				}
				if (!SendToCamera(sock, endln, strlen(endln))){
					continue;
				}
				if (!SendToCamera(sock, cs.cameras [ p ].password, strlen(cs.cameras [ p ].password))){
					continue;
				}
				if (!SendToCamera(sock, endln, strlen(endln))){
					continue;
				}
				memset(bufferCamera, 0, sizeof(bufferCamera));
				read(sock, bufferCamera, sizeof(bufferCamera));
				if (strlen(bufferCamera) == 0) {
					Debug_Message(LOG_ERROR, "Camera%d Ошибка чтения ", id);
					shutdown(sock, SHUT_RDWR);
					close(sock);
					osDelay(1000U);
					continue;
				}
//					Debug_Message(LOG_INFO, "Camera%d прочитали %s", id, buffer);
				if (command==CAMERA_READ_ZONE){
					if (!SendToCamera(sock, gs, strlen(gs))){
						continue;
					}
					memset(bufferCamera, 0, sizeof(bufferCamera));
					read(sock, bufferCamera, sizeof(bufferCamera));
					if (strlen(bufferCamera) == 0) {
						Debug_Message(LOG_ERROR, "Camera%d Ошибка чтения зон", id);
						shutdown(sock, SHUT_RDWR);
						close(sock);
						osDelay(1000U);
						continue;
					}
					appendOneZone(bufferCamera, &zo, p, id);
				}
				if (command==CAMERA_STATISTICS){
					if (!SendToCamera(sock, gdlast, strlen(gdlast))){
						continue;
					}
					while(true){
						memset(bufferCamera, 0, sizeof(bufferCamera));
						read(sock, bufferCamera, sizeof(bufferCamera));
						if (strlen(bufferCamera) == 0) {
							Debug_Message(LOG_ERROR, "Camera%d Ошибка чтения зон", id);
							shutdown(sock, SHUT_RDWR);
							close(sock);
							osDelay(1000U);
							continue;
						}
					}
				}
//				Debug_Message(LOG_INFO, "Camera%d прочитали %s", id, buffer);
				shutdown(sock, SHUT_RDWR);
				close(sock);
			}
		}
		Debug_Message(LOG_INFO, "Camera закончили цикл %s за %d сек",command==CAMERA_READ_ZONE?"чтения зон":"сбора статистики",DiffTimeSecond(start));
	}
}

