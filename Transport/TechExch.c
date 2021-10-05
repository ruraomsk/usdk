/*
 * TechExch.c
 *
 *  Created on: Sep 9, 2021
 *      Author: rura
 */
/*
 * Основной модуль обмена с верхним уровнем
 * Весь обмен построен на двух путном уровне по основному каналу
 * ждем сообщений от сервера
 * по резервному каналу сообщаем об изменениях в своем состоянии
 *
 *
 *
 *
*/
#include "CommonData.h"
#include "Transport.h"
#include "DebugLogger.h"
#include "DeviceLogger.h"
#include "DeviceTime.h"
#include "service.h"
#include "core_json.h"
#include <stdbool.h>
#include <string.h>
#include <strings.h>
#include <stdio.h>
extern osMessageQueueId_t ChangeStatus;
extern osMessageQueueId_t ToServerQueue;
extern osMessageQueueId_t FromServerQueue;
extern osMessageQueueId_t ToServerSecQueue;
extern osMessageQueueId_t FromServerSecQueue;

osTimerId_t TimerTechInterval;
osMessageQueueId_t TimerTechQueue;


MessageFromQueue from;
MessageFromQueue to;

CallBackParam param={.Signal=SIGNAL_NEED_KEEP_ALIVE};

bool mainConnect=false;
bool secConnect=false;
int Interval=20;
int tout=10;
#define MESSAGE_OK "ok"
#define GIVE_ME_STATUS "give_me_status"
#define DIFF_INTERVAL 5
void setTimeoutForChanel(int interval){
	TCPSet tcpSetSec;
	TCPSet tcpSetMain;
	GetCopy("csec", &tcpSetSec);
	GetCopy("cmain", &tcpSetMain);
	tcpSetSec.tque=interval+DIFF_INTERVAL;
	tcpSetMain.tread=interval+DIFF_INTERVAL;
	SetCopy("cmain", &tcpSetMain);
	SetCopy("csec", &tcpSetSec);
}
void prepareConnectMessage(char* message){
	int server_number,interval;
	time_t newTime;
	sscanf(message+sizeof(MESSAGE_OK),"%d,%d,%lld",&server_number,&interval,&newTime);
	GPSSet gps;
	GetCopy("gps", &gps);
	if (!gps.Ok)	UpdateDeviceTime(newTime);
	if (interval!=Interval){
		Interval=interval;
		setTimeoutForChanel(Interval);
	}
}
void prepareGiveMeStatus(char* message){
	time_t newTime;
	sscanf(message+sizeof(GIVE_ME_STATUS),"%lld",&newTime);
	GPSSet gps;
	GetCopy("gps", &gps);
	if (!gps.Ok)	UpdateDeviceTime(newTime);
}
void restartTicker(){
	osTimerStart(TimerTechInterval, (Interval-DIFF_INTERVAL)*1000U);
}
void TechExchange(void *argument){
	setTimeoutForChanel(Interval);
	TimerTechQueue=osMessageQueueNew(6, sizeof(uint32_t), NULL);
	if(TimerTechQueue==NULL){
		Debug_Message(LOG_FATAL, "TechExchange Невозможно создать очередь интервалов");
		return;
	}
	param.QueueId=TimerTechQueue;
	TimerTechInterval=osTimerNew(CallbackQueue, osTimerOnce, &param, NULL);
//	osTimerStart(TimerTechInterval, (Interval-DIFF_INTERVAL)*1000U);
	for(;;){
		vTaskDelay(STEP_CONTROL);
//		Debug_Message(LOG_INFO, "Цикл TechExchange");
		uint32_t signal;
		if (osMessageQueueGet(TimerTechQueue, &signal, NULL, tout)==osOK){
			if(signal==SIGNAL_NEED_KEEP_ALIVE && secConnect) {
				Debug_Message(LOG_INFO, "Начинаем KEEP_ALIVE");
				to.error=TRANSPORT_OK;
				to.message=MessageStatusDevice();
				osMessageQueuePut(ToServerSecQueue, &to, 0, 0);
			}else {
				Debug_Message(LOG_INFO, "Таймер вернул %d",signal);
			}
		}
		if (osMessageQueueGet(FromServerQueue, &from, NULL, tout) == osOK) {
			if (from.error != TRANSPORT_OK) {
				//Сообщение с ошибкой нужно начинать заново
				Debug_Message(LOG_ERROR, "Обмен по основному каналу прекращен");
				mainConnect=false;
				continue;
			}
			if(!mainConnect){
				//Должно быть сообщения коннекта
				if (strncmp(MESSAGE_OK,from.message,strlen(MESSAGE_OK))==0){
					prepareConnectMessage(from.message);
					mainConnect=true;
					to.error=TRANSPORT_OK;
					to.message=MessageConfirm();
					osMessageQueuePut(ToServerQueue, &to, 0, 0);

				}
				vPortFree(from.message);
				continue;
			}
			if (strncmp(GIVE_ME_STATUS,from.message,strlen(GIVE_ME_STATUS))==0){
				prepareGiveMeStatus(from.message);
				to.message=MessageStatusDevice();
				osMessageQueuePut(ToServerQueue, &to, 0, 0);
				vPortFree(from.message);
				continue;
			}
			Debug_Message(LOG_INFO, "Главное сообщение не обработано %.60s",from.message);
			vPortFree(from.message);

		}
		if (osMessageQueueGet(FromServerSecQueue, &from, NULL, tout) == osOK) {
			if (from.error != TRANSPORT_OK) {
//				Сообщение с ошибкой нужно начинать заново
				Debug_Message(LOG_ERROR, "Обмен по второму каналу прекращен");
				secConnect=false;
				continue;
			}
			if(!secConnect){
				//Должно быть сообщения коннекта
				if (strncmp(MESSAGE_OK,from.message,strlen(MESSAGE_OK))==0){
					secConnect=true;
					prepareConnectMessage(from.message);
					to.error=TRANSPORT_OK;
					to.message=MessageConfirm();
					osMessageQueuePut(ToServerSecQueue, &to, 0, 0);
					restartTicker();
				}
				vPortFree(from.message);
				continue;
			}
			if (strncmp(MESSAGE_OK,from.message,strlen(MESSAGE_OK))==0){
				Debug_Message(LOG_INFO, "Перезапуск таймера");
				restartTicker();
				vPortFree(from.message);
				continue;
			}
			Debug_Message(LOG_INFO, "Сообщение  по второму каналу не обработано %.60s",from.message);
			vPortFree(from.message);
			continue;
		}
	}
}
