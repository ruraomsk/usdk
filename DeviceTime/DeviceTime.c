/*
 * DeviceTime.c
 *
 *  Created on: Aug 19, 2021
 *      Author: rura
 */
#include "main.h"
#include "DeviceTime.h"

static dev_time DeviceTimer;
static char DeviceTimeBuffer[20];
static osMutexId_t TimerMutex;
void DeviceTimeInit(){
	//Здесь потом напишем
	DeviceTimer=0;
	TimerMutex=osMutexNew(NULL);
}
dev_time GetDeviceTime(){
	UpdateDeviceTime();
	return DeviceTimer;
}
char* TimeToString(dev_time time){
	if (osMutexAcquire(TimerMutex, osWaitForever) == osOK) {
		int hour, minute, sec, msec;
		msec = time % 1000;
		sec = (time / 1000U) % 86400U;
		hour = sec / 3600;
		sec -= hour * 3600;
		minute = sec / 60;
		sec = sec % 60;
		sprintf(DeviceTimeBuffer, "%02d.%02d.%02d.%03d", hour, minute, sec, msec);
	} else {
		char temp[]="ERROR!\0";
		return temp;
	}
	osMutexRelease(TimerMutex);
	return DeviceTimeBuffer;
}
void UpdateDeviceTime(void){
	DeviceTimer=HAL_GetTick();
}