/*
 * Common_private.h
 *
 *  Created on: Sep 17, 2021
 *      Author: rura
 */

#ifndef COMMON_PRIVATE_H_
#define COMMON_PRIVATE_H_

#include "CommonData.h"

typedef struct {
	char *name;
	void *data;
	size_t size;
	size_t sbuf;
	char* (*to_json) (void *,size_t);
	void (*from_json) (char *,void *);
	void (*by_default) (void *);
}CommonData;


void clearSetupDK(SetupDK *setupDK);
char *SetupDKToJsonString(SetupDK *setupDK,size_t size);
void SetupDKFromJsonString(char* root, SetupDK *setupDK);

void clearWeekSet(WeekSet *weekSet);
char WeekSetToJsonString(WeekSet *weekSet,size_t size);
void WeekSetFromJsonString(char* root, WeekSet *weekSet);

void clearDaySet(DaySet *daySet);
char* DaySetToJsonString(DaySet *daySet,size_t size);
void DaySetFromJsonString(char *root, DaySet *daySet);

void clearPhasesSet(PhasesSet* phasesSet);
char* PhasesSetToJsonString(PhasesSet *phasesSet,size_t size);
void PhasesSetFromJsonString(char *root, PhasesSet *phasesSet);

void clearYearSet(YearSet *yearSet);
char* YearSetToJsonString(YearSet *yearSet,size_t size);
void YearSetFromJsonString(char* root, YearSet *yearSet);

void clearDeviceStatus(DeviceStatus *ds);
char* DeviceStatusToJsonString(DeviceStatus *ds,size_t size);
void DeviceStatusFromJsonString(char *root, DeviceStatus *ds);

void clearTCPSet(TCPSet *t);
char* TCPSetToJsonString(TCPSet *t,size_t size);
void TCPSetFromJsonString(char *root, TCPSet *t);

#endif /* COMMON_PRIVATE_H_ */
