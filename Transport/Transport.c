/*
 * Transport.c
 *
 *  Created on: 30 авг. 2021 г.
 *      Author: rura
 */

#include "cmsis_os.h"

#include "DeviceLogger.h"
#include "DebugLogger.h"
#include "Transport.h"
//DeviceStatus deviceStatus;

//Очереди для Ethernet
extern osMessageQueueId_t ToServerQueue;
extern osMessageQueueId_t FromServerQueue;
extern osMessageQueueId_t ToServerSecQueue;
extern osMessageQueueId_t FromServerSecQueue;

//Очереди для GPRS
extern osMessageQueueId_t GPRSToServerQueue;
extern osMessageQueueId_t GPRSFromServerQueue;
extern osMessageQueueId_t GPRSToServerSecQueue;
extern osMessageQueueId_t GPRSFromServerSecQueue;

//Очереди от основной программы управления
extern osMessageQueueId_t MainChangeStatus;

extern osMessageQueueId_t MainToServerQueue;
extern osMessageQueueId_t MainFromServerQueue;
extern osMessageQueueId_t MainToServerSecQueue;
extern osMessageQueueId_t MainFromServerSecQueue;



extern int GPRSNeed;
extern int TransportNeed;

#define tout 10
void startGPRS(){
	MessageFromQueue 	msg;
	int c=1;
	msg.error=TRANSPORT_STOP;
	osMessageQueuePut(ToServerQueue,&msg,0,0);
	osMessageQueuePut(ToServerSecQueue,&msg,0,0);
	osMessageQueuePut(MainChangeStatus,&c,0,0);
	Debug_Message(LOG_INFO, "Переключились с ETH на GPRS");
	GPRSNeed=1;
}
void stopGPRS(){
	MessageFromQueue 	msg;
	int c=0;
	msg.error=TRANSPORT_STOP;
	osMessageQueuePut(GPRSToServerQueue,&msg,0,0);
	osMessageQueuePut(GPRSToServerSecQueue,&msg,0,0);
	osMessageQueuePut(MainChangeStatus,&c,0,0);
	Debug_Message(LOG_INFO, "Переключились с GPRS на ETH");
	GPRSNeed=0;

}
void noETHandGPRS(){
	MessageFromQueue 	msg;
	int c=-1;
	osMessageQueuePut(MainChangeStatus,&c,0,0);
	Debug_Message(LOG_INFO, "Нет устройств связи");
	for(;;){
		if (osMessageQueueGet(MainToServerQueue, &msg, NULL, tout) == osOK) {
			Debug_Message(LOG_INFO, "Пришло сообщение на главный ");

		}
		if (osMessageQueueGet(MainToServerSecQueue, &msg, NULL, tout) == osOK) {
			Debug_Message(LOG_INFO, "Пришло сообщение на второй ");
		}
	}
}
void mainTransportLoop(void){
	ToServerQueue = osMessageQueueNew(16, sizeof(MessageFromQueue), NULL);
	FromServerQueue = osMessageQueueNew(16, sizeof(MessageFromQueue), NULL);
	ToServerSecQueue = osMessageQueueNew(16, sizeof(MessageFromQueue), NULL);
	FromServerSecQueue = osMessageQueueNew(16, sizeof(MessageFromQueue), NULL);

	GPRSToServerQueue = osMessageQueueNew(16, sizeof(MessageFromQueue), NULL);
	GPRSFromServerQueue = osMessageQueueNew(16, sizeof(MessageFromQueue), NULL);
	GPRSToServerSecQueue = osMessageQueueNew(16, sizeof(MessageFromQueue), NULL);
	GPRSFromServerSecQueue = osMessageQueueNew(16, sizeof(MessageFromQueue), NULL);

	MainToServerQueue = osMessageQueueNew(16, sizeof(MessageFromQueue), NULL);
	MainFromServerQueue = osMessageQueueNew(16, sizeof(MessageFromQueue), NULL);
	MainToServerSecQueue = osMessageQueueNew(16, sizeof(MessageFromQueue), NULL);
	MainFromServerSecQueue = osMessageQueueNew(16, sizeof(MessageFromQueue), NULL);

	//0 - Переход от GPRS на Ethernet
	//1 - Переход от Ethernet на GPRS
	//-1 - Нет связи с внешним миром

	MainChangeStatus = osMessageQueueNew(16, sizeof(int), NULL);

	TransportNeed=1;

	DeviceStatus deviceStatus=readSetup( "setupdevice");
	if(!deviceStatus.Ethertnet && !deviceStatus.Gprs) {
		noETHandGPRS();
	}

	MessageFromQueue 	msg;
	for(;;){
		//Вначале ппроверяем ответы по Ethernet
		if (osMessageQueueGet(FromServerQueue, &msg, NULL, tout) == osOK) {
			if (msg.error==TRANSPORT_OK){
				if (GPRSNeed == 1) {
					//Работали по GPRS нужно его остановить и перейти на Ethernet
					stopGPRS();
				}
				osMessageQueuePut(MainFromServerQueue,&msg,0,0);
			} else {
				if (GPRSNeed ==0 ) {
					//Работали по Ethernet нужно запустить GPRS
					startGPRS();
				}
			}
		}
		if (osMessageQueueGet(FromServerSecQueue, &msg, NULL, tout) == osOK) {
			if (msg.error==TRANSPORT_OK){
				if (GPRSNeed == 1) {
					//Работали по GPRS нужно его остановить и перейти на Ethernet
					stopGPRS();
				}
				osMessageQueuePut(MainFromServerSecQueue,&msg,0,0);
			} else {
				if (GPRSNeed ==0 ) {
					//Работали по Ethernet нужно запустить GPRS
					startGPRS();
				}
			}
		}
		if (GPRSNeed==1){
			//Вычитываем и пробрасываем сообщения от GPRS
			if (osMessageQueueGet(GPRSFromServerQueue, &msg, NULL, tout) == osOK) {
					osMessageQueuePut(MainFromServerQueue,&msg,0,0);
			}
			if (osMessageQueueGet(GPRSFromServerSecQueue, &msg, NULL, tout) == osOK) {
					osMessageQueuePut(MainFromServerSecQueue,&msg,0,0);
			}
		} else {
			//Вычитываем и отбрасываем сообщения от GPRS
			osMessageQueueGet(GPRSFromServerQueue, &msg, NULL, tout);
			osMessageQueueGet(GPRSFromServerSecQueue, &msg, NULL, tout);
		}
		// Пересылаем сообщения от главного модуля
		if (osMessageQueueGet(MainToServerQueue, &msg, NULL, tout) == osOK) {
			if (GPRSNeed==0) osMessageQueuePut(ToServerQueue,&msg,0,0);
			else osMessageQueuePut(GPRSToServerQueue,&msg,0,0);
		}
		if (osMessageQueueGet(MainToServerSecQueue, &msg, NULL, tout) == osOK) {
			if (GPRSNeed==0) osMessageQueuePut(ToServerSecQueue,&msg,0,0);
			else osMessageQueuePut(GPRSToServerSecQueue,&msg,0,0);
		}
	}
}
