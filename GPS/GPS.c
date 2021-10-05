/*
 * GPS.C
 *
 *  Created on: Oct 4, 2021
 *      Author: rura
 */

#include "CommonData.h"
#include "DebugLogger.h"
#include "cmsis_os.h"

osThreadId_t GPSCtlHandle;
const osThreadAttr_t GPSCtl_attributes = { .name = "GPS", .stack_size = 1024 * 4, .priority =
		(osPriority_t) osPriorityLow, };
void StartGPSCtl (void *arg){
	GPSSet gps;
	GetCopy("gps", &gps);
	//INIT GPS
	Debug_Message(LOG_INFO, "Модуль GPS запущен");
	for (;;){
		gps.Seek=true;
		gps.Ok=false;
		osDelay(1000U);
	}
}

void GPS_Init() {
	DeviceStatus ds;
	GetCopy("setup",&ds);
	if (ds.Gps) {
		GPSCtlHandle = osThreadNew(StartGPSCtl, NULL, &GPSCtl_attributes);
	}
}

