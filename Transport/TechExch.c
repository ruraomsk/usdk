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

extern osMessageQueueId_t MainChangeStatus;
extern osMessageQueueId_t MainToServerQueue;
extern osMessageQueueId_t MainFromServerQueue;
extern osMessageQueueId_t MainToServerSecQueue;
extern osMessageQueueId_t MainFromServerSecQueue;

void TechExchange(void *argument){

	for(;;){

	}
}
