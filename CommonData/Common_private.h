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


void clearSetupDK(void *setupDK);
char *SetupDKToJsonString(void *setupDK,char* buffer,size_t size);
void SetupDKFromJsonString(char* root, void *setupDK);

void clearWeekSet(void *weekSet);
char WeekSetToJsonString(void *weekSet,char* buffer,size_t size);
void WeekSetFromJsonString(char* root, void *weekSet);
char* OneWeekToJsonString(void *weekSet,int week,char* buffer,size_t size);
bool OneWeekFromJsonString(char* root, WeekSet *weekSet,int week);

void clearDaySet(void *daySet);
char* DaySetToJsonString(void *daySet,char* buffer,size_t size);
void DaySetFromJsonString(char *root, void *daySet);
char* OneDayToJsonString(void *daySet,int day,char* buffer,size_t size);
bool OneDayFromJsonString(char *root, DaySet *daySet,int day);

void clearPhasesSet(void* phasesSet);
char* PhasesSetToJsonString(void *phasesSet,char* buffer,size_t size);
void PhasesSetFromJsonString(char *root, void *phasesSet);
char* OnePhaseToJsonString(void *phasesSet,int phase,char* buffer,size_t size);
bool OnePhaseFromJsonString(char *root, PhasesSet *phasesSet,int phase);

void clearYearSet(void *yearSet);
char* YearSetToJsonString(void *yearSet,char* buffer,size_t size);
void YearSetFromJsonString(char* root, void *yearSet);
char* OneMonthToJsonString(void *yearSet,int month,char* buffer,size_t size);
bool OneMonthFromJsonString(char* root, YearSet *yearSet,int month);

void clearDeviceStatus(void *ds);
char* DeviceStatusToJsonString(void *ds,char* buffer,size_t size);
void DeviceStatusFromJsonString(char *root, void *ds);

void clearTCPSet(void *t);
char* TCPSetToJsonString(void *t,char* buffer,size_t size);
void TCPSetFromJsonString(char *root, void *t);

void clearTimeDevice(void *t);
char* TimeDeviceToJsonString(void *t,char* buffer,size_t size);
void TimeDeviceFromJsonString(char *root, void *t);

void clearGPSSet(void *g);
char* GPSSetToJsonString(void *g,char* buffer,size_t size);
void GPSSetFromJsonString(char *root, void *g);

void clearCameraSet(void *c);
char* CameraSetToJsonString(void *c,char* buffer,size_t size);
void CameraSetFromJsonString(char *root, void *c);

void clearAllPks(void *c);
char* AllPksToJsonString(void *c,char* buffer,size_t size);
void AllPksFromJsonString(char *root, void *c);
char* OnePKToJsonString(void *apks,int pk,char* buffer,size_t size);
bool OnePKFromJsonString(char *root, AllPks *apks,int pk);

void clearNowState(void *ns);
char* NowStateToJsonString(void *ns,char* buffer,size_t size);
void NowStateFromJsonString(char *root, void *ns);

#endif /* COMMON_PRIVATE_H_ */
