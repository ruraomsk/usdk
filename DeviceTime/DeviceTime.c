/*
 * DeviceTime.c
 *
 *  Created on: Aug 19, 2021
 *      Author: rura
 */
#include "main.h"
#include "DebugLogger.h"
#include "DeviceTime.h"
#include "CommonData.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

osTimerId_t DeviceTimerIncrement;

static time_t DeviceTimer;
static uint32_t oldTicks=0;
void incrementDeviceTime(void *arg) {
	DeviceTimer+=(HAL_GetTick()-oldTicks)/1000U;
	oldTicks=HAL_GetTick();
}
void DeviceTimeInit() {
	//Здесь потом напишем
	DeviceTimer = 0;
	DeviceTimerIncrement = osTimerNew(incrementDeviceTime, osTimerPeriodic, NULL, NULL);
	osTimerStart(DeviceTimerIncrement, 1000U);
}
long int DiffTimeSecond(time_t start) {
	return (DeviceTimer - start);
}
time_t GetDeviceTime() {
	return DeviceTimer;
}
void UpdateDeviceTime(time_t time) {
	struct tm *u;
	u=localtime(&time);
	TimeDevice td;
	GetCopy(TimeDeviceName, &td);
	u->tm_hour+=td.TimeZone;
	time_t new = mktime(u);
	if(llabs(DiffTimeSecond(new))>2) DeviceTimer=new;
}
char bufferTime [ 30 ];

char* TimeToString(time_t time) {
	struct tm *u;
	u = localtime(&time);
	strftime(bufferTime, sizeof(bufferTime), "%d.%m.%Y %H:%M:%S", u);
	return bufferTime;
}
char* ShortTimeToString(time_t time) {
	struct tm *u;
	u = localtime(&time);
	strftime(bufferTime, sizeof(bufferTime), "%H:%M:%S", u);
	return bufferTime;
}
bool isTimeCorrect(){
	time_t time=GetDeviceTime();
	struct tm *u;
	u = localtime(&time);
	return u->tm_year>2020;
}

int nanosleep(const struct timespec *tw, struct timespec *tr) {
	unsigned long int delay = ((unsigned long int) tw->tv_sec) * 1000UL + ((unsigned long int) tw->tv_nsec / 1000UL);
	return osDelay(delay);
}
void CallbackQueue(void *arg) {
	CallBackParam *par = (CallBackParam*) arg;
	osMessageQueuePut(par->QueueId, &par->Signal, 0, osWaitForever);
}
