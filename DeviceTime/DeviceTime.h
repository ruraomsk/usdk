/*
 * DeviceTime.h
 *
 *  Created on: Aug 19, 2021
 *      Author: rura
 */

#ifndef DEVICETIME_H_
#define DEVICETIME_H_

#include "cmsis_os.h"
#include <time.h>
#include <stdbool.h>
#include <stdint.h>
#define TimeSecond 1000U
#define TimeMinute (60*TimeSecond)
#define TimeHour   (TimeMinute*60)
#define DeviceTimeStep 100
typedef unsigned long int dev_time;
typedef struct {
	osMessageQueueId_t QueueId;
	uint32_t Signal;
}CallBackParam;
void DeviceTimeInit();
dev_time GetDeviceTime(void);
char* TimeToString(dev_time time);
void UpdateDeviceTime(void);
int nanosleep (const struct timespec *tw, struct timespec *tr);
void CallbackQueue(void* arg);
int DiffTimeSecond(dev_time start);

#endif /* DEVICETIME_H_ */
