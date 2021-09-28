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
#include "service.h"
#include "core_json.h"
#include <stdbool.h>
#include <string.h>
#include <strings.h>
extern osMessageQueueId_t ChangeStatus;
extern osMessageQueueId_t ToServerQueue;
extern osMessageQueueId_t FromServerQueue;
extern osMessageQueueId_t ToServerSecQueue;
extern osMessageQueueId_t FromServerSecQueue;

MessageFromQueue from;
MessageFromQueue to;
bool mainConnect=false;
bool secConnect=false;
int tout=10;
char rString[80];
int interval=-1;
bool controlInterbal=false;
int count=-1;
#define MESSAGE_OK "ok"
#define GIVE_ME_STATUS "give_me_status"
void TechExchange(void *argument){
	for(;;){
		vTaskDelay(100);
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
					mainConnect=true;
					to.error=TRANSPORT_OK;
					to.message=MessageConfirm();
					osMessageQueuePut(ToServerQueue, &to, 0, 0);

				}
				vPortFree(from.message);
				continue;
			}
			if (strncmp(GIVE_ME_STATUS,from.message,strlen(GIVE_ME_STATUS))==0){
				to.message=MessageStatusDevice();
				osMessageQueuePut(ToServerQueue, &to, 0, 0);
				vPortFree(from.message);
				continue;
			}
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
					mainConnect=true;
					to.error=TRANSPORT_OK;
					to.message=MessageConfirm();
					osMessageQueuePut(ToServerQueue, &to, 0, 0);

				}
				vPortFree(from.message);
				continue;
			}
//			doSecondMessage(from);
		}


	}
}
