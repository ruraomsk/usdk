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

typedef struct {
	osMessageQueueId_t QueueId;
	uint32_t Signal;
}CallBackParam;

void DeviceTimeInit();
time_t GetDeviceTime(void);

char* TimeToString(time_t time);
char* ShortTimeToString(time_t time);

void UpdateDeviceTime(time_t time);
int nanosleep (const struct timespec *tw, struct timespec *tr);
void CallbackQueue(void* arg);
long int DiffTimeSecond(time_t start);

#endif /* DEVICETIME_H_ */
