/*
 * DeviceTime.c
 *
 *  Created on: Aug 19, 2021
 *      Author: rura
 */
#include "main.h"
#include "DebugLogger.h"
#include "DeviceTime.h"
#include <string.h>
#include <stdio.h>

static dev_time DeviceTimer;
static char DeviceTimeBuffer[20];
static osMutexId_t TimerMutex;
void DeviceTimeInit() {
	//Здесь потом напишем
	DeviceTimer = 0;
	TimerMutex = osMutexNew(NULL);
}
int DiffTimeSecond(dev_time start){
	return (GetDeviceTime()-start)/1000U;
}
dev_time GetDeviceTime() {
	UpdateDeviceTime();
	return DeviceTimer;
}
char* TimeToString(dev_time time) {
	if (osMutexAcquire(TimerMutex, osWaitForever) == osOK) {
		int hour, minute, sec, msec;
		msec = time % 1000;
		sec = (time / 1000U) % 86400U;
		hour = sec / 3600;
		sec -= hour * 3600;
		minute = sec / 60;
		sec = sec % 60;
		sprintf(DeviceTimeBuffer, "%02d.%02d.%02d.%03d", hour, minute, sec, msec);
		osMutexRelease(TimerMutex);
		return DeviceTimeBuffer;
	}
	return "ERROR!\0";
}
void UpdateDeviceTime(void) {
	DeviceTimer = HAL_GetTick();
}
int nanosleep(const struct timespec *tw, struct timespec *tr) {
	unsigned long int delay = ((unsigned long int) tw->tv_sec) * 1000UL + ((unsigned long int) tw->tv_nsec / 1000UL);
	return osDelay(delay);
}

void CallbackQueue(void* arg){
	CallBackParam* par=(CallBackParam *)arg;
	uint32_t signal=par->Signal;
//	uint32_t size= osMessageQueueGetMsgSize(QueueId);
//	void* buffer=pvPortMalloc(size);
//	if (buffer==NULL){
//		Debug_Message(LOG_ERROR, "Нет %d памяти для CallbackQueue",size);
//		return;
//	}
//	memset(buffer,0,size);
	osMessageQueuePut(par->QueueId,&signal , 0, 0);
}
