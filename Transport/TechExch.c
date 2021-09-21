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

#include "Transport.h"
#include "DebugLogger.h"
#include "DeviceLogger.h"
#include "core_json.h"
#include <stdbool.h>
extern osMessageQueueId_t MainChangeStatus;
extern osMessageQueueId_t MainToServerQueue;
extern osMessageQueueId_t MainFromServerQueue;
extern osMessageQueueId_t MainToServerSecQueue;
extern osMessageQueueId_t MainFromServerSecQueue;

MessageFromQueue from;
MessageFromQueue to;
bool mainConnect=false;
bool secConnect=false;
int tout=10;
char* MessageOk (){
	js_write w;
	js_write_start(&w, 120);
	js_write_value_start(&w, "confirm");
	js_write_bool(&w, "status", true);
	js_write_string(&w, "error", "No error");
	js_write_value_end(&w);
	js_write_end(&w);
	return w.start;
}
void TechExchange(void *argument){

	for(;;){
		osDelay(100);
		if (osMessageQueueGet(MainFromServerQueue, &from, NULL, tout) == osOK) {
			if (from.error != TRANSPORT_OK) {
				//Сообщение с ошибкой нужно начинать заново
				mainConnect=false;
				vPortFree(from.message);
				continue;
			}
			if(!mainConnect){
				//Должно быть сообщения коннекта
				js_read r,v;
				js_read_start(&r, from.message);
				if (js_read_value(&r, "connect", &v)==JsonSuccess){
					to.error=TRANSPORT_OK;
					to.message=MessageOk();
					osMessageQueuePut(MainToServerQueue, &to, 0, 0);
				}
			}
			vPortFree(from.message);
			continue;
		}
		if (osMessageQueueGet(MainFromServerSecQueue, &from, NULL, tout) == osOK) {
			if (from.error != TRANSPORT_OK) {
//				Сообщение с ошибкой нужно начинать заново
				secConnect=false;
				vPortFree(from.message);
				continue;
			}
			if(!secConnect){
				//Должно быть сообщения коннекта
				js_read r,v;
				js_read_start(&r, from.message);
				if (js_read_value(&r, "connect", &v)==JsonSuccess){
//					to.error=TRANSPORT_OK;
//					to.message=MessageOk();
//					osMessagePut(MainToServerSecQueue, &to, 0,0);

				}
			}
			vPortFree(from.message);
			continue;
		}


	}
}
