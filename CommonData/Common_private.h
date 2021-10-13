/*
 * Common_private.h
 *
 *  Created on: Sep 17, 2021
 *      Author: rura
 */

#ifndef COMMON_PRIVATE_H_
#define COMMON_PRIVATE_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "CommonData.h"

typedef struct {
	char *name;
	void *data;
	size_t size;
	size_t sbuf;
	bool change;
	char* (*to_json) (void *,char *,size_t);
	void (*from_json) (char *,void *);
	void (*by_default) (void *);
}CommonData;


void clearSetupDK(SetupDK *setupDK);
char *SetupDKToJsonString(SetupDK *setupDK,char* buffer,size_t size);
void SetupDKFromJsonString(char* root, SetupDK *setupDK);

void clearWeekSet(WeekSet *weekSet);
char WeekSetToJsonString(WeekSet *weekSet,char* buffer,size_t size);
void WeekSetFromJsonString(char* root, WeekSet *weekSet);
char* OneWeekToJsonString(WeekSet *weekSet,int week,char* buffer,size_t size);

void clearDaySet(DaySet *daySet);
char* DaySetToJsonString(DaySet *daySet,char* buffer,size_t size);
void DaySetFromJsonString(char *root, DaySet *daySet);
char* OneDayToJsonString(DaySet *daySet,int day,char* buffer,size_t size);

void clearPhasesSet(PhasesSet* phasesSet);
char* PhasesSetToJsonString(PhasesSet *phasesSet,char* buffer,size_t size);
void PhasesSetFromJsonString(char *root, PhasesSet *phasesSet);

void clearYearSet(YearSet *yearSet);
char* YearSetToJsonString(YearSet *yearSet,char* buffer,size_t size);
void YearSetFromJsonString(char* root, YearSet *yearSet);
char* OneMonthToJsonString(YearSet *yearSet,int month,char* buffer,size_t size);

void clearDeviceStatus(DeviceStatus *ds);
char* DeviceStatusToJsonString(DeviceStatus *ds,char* buffer,size_t size);
void DeviceStatusFromJsonString(char *root, DeviceStatus *ds);

void clearTCPSet(TCPSet *t);
char* TCPSetToJsonString(TCPSet *t,char* buffer,size_t size);
void TCPSetFromJsonString(char *root, TCPSet *t);

void clearTimeDevice(TimeDevice *t);
char* TimeDeviceToJsonString(TimeDevice *t,char* buffer,size_t size);
void TimeDeviceFromJsonString(char *root, TimeDevice *t);

void clearGPSSet(GPSSet *g);
char* GPSSetToJsonString(GPSSet *g,char* buffer,size_t size);
void GPSSetFromJsonString(char *root, GPSSet *g);

void clearCameraSet(CameraSet *c);
char* CameraSetToJsonString(CameraSet *c,char* buffer,size_t size);
void CameraSetFromJsonString(char *root, CameraSet *c);

void clearAllPks(AllPks *c);
char* AllPksToJsonString(AllPks *c,char* buffer,size_t size);
void AllPksFromJsonString(char *root, AllPks *c);
char* OnePKToJsonString(AllPks *apks,int pk,char* buffer,size_t size);

void clearNowState(NowState *ns);
char* NowStateToJsonString(NowState *ns,char* buffer,size_t size);
void NowStateFromJsonString(char *root, NowState *ns);

#endif /* COMMON_PRIVATE_H_ */
