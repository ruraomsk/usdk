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

CameraSet cs;
char endln[]="\r\n";
char gs[]="gs\r\n";
void OneCameraWork(void* arg){
	int id=*(int*) arg;
	char buffer[60];
	int socket=-1;
	int p=-1;
	for (int i = 0; i < MAX_CAMERAS; ++i) {
		if (cs.cameras[i].id==id){
			p=i;
			break;
		}
	}
	if(p<0) {
		Debug_Message(LOG_FATAL, "Не найден ID=%d",id);
		return;
	}

	for(;;){
		osDelay(1000U);
		while(socket<0) {
			osDelay(1000U);
			struct sockaddr_in srv_addr;
			inet_aton(cs.cameras[p].ip, &srv_addr.sin_addr.s_addr);
			srv_addr.sin_family = AF_INET;
			srv_addr.sin_port = htons(cs.cameras[p].port);
			socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
			if (socket < 0) {
				Debug_Message(LOG_ERROR,"Camera%d Не могу создать сокет %d",id, errno);
				continue;
			}
			int err = connect(socket, (struct sockaddr* ) &srv_addr, sizeof(struct sockaddr_in));
			if (err != 0) {
				Debug_Message(LOG_ERROR, "Camera%d Нет соединения ",id);
				shutdown(socket, SHUT_RDWR);
				close(socket);
				socket=-1;
				continue;
			}
			send(socket,cs.cameras[p].login,strlen(cs.cameras[p].login),0);
			send(socket,endln,strlen(endln),0);
			send(socket,cs.cameras[p].password,strlen(cs.cameras[p].password),0);
			send(socket,endln,strlen(endln),0);
			memset(buffer,0,sizeof(buffer));
			read(socket,buffer,sizeof(buffer));
			if(strlen(buffer)==0){
				Debug_Message(LOG_ERROR, "Camera%d Ошибка чтения ",id);
				shutdown(socket, SHUT_RDWR);
				close(socket);
				socket=-1;
				continue;
			}
			Debug_Message(LOG_INFO, "Camera%d прочитали %s",id,buffer);

		}
		send(socket,gs,strlen(gs),0);
		memset(buffer,0,sizeof(buffer));
		read(socket,buffer,sizeof(buffer));
		if(strlen(buffer)==0){
			Debug_Message(LOG_ERROR, "Camera%d Ошибка чтения статистики",id);
			shutdown(socket, SHUT_RDWR);
			close(socket);
			socket=-1;
			continue;
		}
		Debug_Message(LOG_INFO, "Camera%d прочитали %s",id,buffer);
	}

}

osThreadId_t CamerasHandle;
const osThreadAttr_t Camera01_attributes = { .name = "Camera01", .stack_size = 2048 * 4, .priority =
		(osPriority_t) osPriorityLow, };
const osThreadAttr_t Camera02_attributes = { .name = "Camera02", .stack_size = 2048 * 4, .priority =
		(osPriority_t) osPriorityLow, };
const osThreadAttr_t Camera03_attributes = { .name = "Camera03", .stack_size = 2048 * 4, .priority =
		(osPriority_t) osPriorityLow, };
const osThreadAttr_t Camera04_attributes = { .name = "Camera04", .stack_size = 2048 * 4, .priority =
		(osPriority_t) osPriorityLow, };
const osThreadAttr_t Camera05_attributes = { .name = "Camera05", .stack_size = 2048 * 4, .priority =
		(osPriority_t) osPriorityLow, };
const osThreadAttr_t Camera06_attributes = { .name = "Camera06", .stack_size = 2048 * 4, .priority =
		(osPriority_t) osPriorityLow, };
const osThreadAttr_t Camera07_attributes = { .name = "Camera07", .stack_size = 2048 * 4, .priority =
		(osPriority_t) osPriorityLow, };
const osThreadAttr_t Camera08_attributes = { .name = "Camera08", .stack_size = 2048 * 4, .priority =
		(osPriority_t) osPriorityLow, };
const osThreadAttr_t Camera09_attributes = { .name = "Camera09", .stack_size = 2048 * 4, .priority =
		(osPriority_t) osPriorityLow, };
const osThreadAttr_t Camera10_attributes = { .name = "Camera10", .stack_size = 2048 * 4, .priority =
		(osPriority_t) osPriorityLow, };
const osThreadAttr_t Camera11_attributes = { .name = "Camera11", .stack_size = 2048 * 4, .priority =
		(osPriority_t) osPriorityLow, };
const osThreadAttr_t Camera12_attributes = { .name = "Camera12", .stack_size = 2048 * 4, .priority =
		(osPriority_t) osPriorityLow, };
const osThreadAttr_t Camera13_attributes = { .name = "Camera13", .stack_size = 2048 * 4, .priority =
		(osPriority_t) osPriorityLow, };
const osThreadAttr_t Camera14_attributes = { .name = "Camera14", .stack_size = 2048 * 4, .priority =
		(osPriority_t) osPriorityLow, };
const osThreadAttr_t Camera15_attributes = { .name = "Camera15", .stack_size = 2048 * 4, .priority =
		(osPriority_t) osPriorityLow, };
const osThreadAttr_t Camera16_attributes = { .name = "Camera16", .stack_size = 2048 * 4, .priority =
		(osPriority_t) osPriorityLow, };
// @formatter:off

const osThreadAttr_t* catr[16]={
							&Camera01_attributes,&Camera02_attributes,&Camera03_attributes,&Camera04_attributes,&Camera05_attributes,
							&Camera06_attributes,&Camera07_attributes,&Camera08_attributes,&Camera09_attributes,&Camera10_attributes,
							&Camera11_attributes,&Camera12_attributes,&Camera13_attributes,&Camera14_attributes,&Camera15_attributes,
							&Camera16_attributes
						 	 	 };
// @formatter:on

void initCameras() {
	osDelay(1000U);
	GetCopy("cam", &cs);
	for (int i = 0; i < 1; ++i) {
		if (cs.cameras[i].id!=0){
			CamerasHandle=osThreadNew(OneCameraWork, (void *)&cs.cameras[i].id , catr[i]);
		}
	}
}

