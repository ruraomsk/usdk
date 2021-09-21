/*
 * CommonData.c
 *
 *  Created on: 13 сент. 2021 г.
 *      Author: rura
 */

#include "CommonData.h"
#include "Files.h"
#include "DeviceLogger.h"
#include "core_json.h"
#include "Common_private.h"
#include <stdio.h>
#include <string.h>

PhasesSet phasesSet;
SetupDK setupDK;
YearSet yearSet;
WeekSet weekSet;
DaySet daySet;
DeviceStatus deviceStatus;
TCPSet mainTCP;
TCPSet secondTCP;

osMutexId_t commonMutex;
// @formatter:off
CommonData cD[] = {
		{ .name = "phases", .data = &phasesSet, .size = sizeof(PhasesSet), .sbuf = 1200, .to_json =	&PhasesSetToJsonString, .from_json = &PhasesSetFromJsonString,.clear=&clearPhasesSet},
		{ .name = "setdk", .data = &setupDK, .size =sizeof(SetupDK), .sbuf = 120, .to_json = &SetupDKToJsonString, .from_json = &SetupDKFromJsonString,.clear=&clearSetupDK},
		{ .name = "year", .data = &yearSet, .size = sizeof(YearSet), .sbuf = 1200, .to_json =&YearSetToJsonString, .from_json =&YearSetFromJsonString,.clear=&clearYearSet },
		{ .name = "week", .data = &weekSet, .size = sizeof(WeekSet), .sbuf = 1200, .to_json =&WeekSetToJsonString, .from_json = &WeekSetFromJsonString,.clear=&clearWeekSet},
		{ .name = "day", .data = &daySet, .size =sizeof(DaySet), .sbuf = 4800, .to_json = &DaySetToJsonString, .from_json = &DaySetFromJsonString,.clear=&clearDaySet },
		{ .name = "setup", .data = &deviceStatus, .size =sizeof(DeviceStatus), .sbuf = 1200, .to_json = &DeviceStatusToJsonString, .from_json = &DeviceStatusFromJsonString,.clear=&clearDeviceStatus},
		{ .name="cmain",.data=&mainTCP,.size=sizeof(TCPSet),.sbuf=80,.to_json=&TCPSetToJsonString,.from_json=&TCPSetFromJsonString,.clear=&clearTCPSet},
		{ .name="csec",.data=&secondTCP,.size=sizeof(TCPSet),.sbuf=80,.to_json=&TCPSetToJsonString,.from_json=&TCPSetFromJsonString,.clear=&clearTCPSet},
		{ .name =NULL }
};
// @formatter:on
char path [ 60 ];
void initCommonData(void) {
	commonMutex=osMutexNew(NULL);
	for (int i = 0;; ++i) {
		if (cD [ i ].name == NULL) break;
		cD [ i ].clear(cD [ i ].data);
		snprintf(path, sizeof(path), "set/%s.set", cD [ i ].name);
		char *json = FilesGetJsonString(path);
		if (json == NULL) {
			DeviceLog(SUB_FILES, "Нет данных %s", path);
			json = cD [ i ].to_json(cD [ i ].data, cD [ i ].sbuf);
			if (json != NULL) {
				FilesSetJsonString(path, json);
			}
		}
		cD [ i ].from_json(json, cD [ i ].data);
		vPortFree(json);
	}
}
int Compare(char *name, void *data) {
	int result = -1;
	if (osMutexAcquire(commonMutex, osWaitForever) == osOK) {
		for (int i = 0;; ++i) {
			if (cD [ i ].name == NULL) break;
			if (strcmp(name, cD [ i ].name) == 0) {
				result = memcmp(data, cD [ i ].data, cD [ i ].size);
				break;
			}
		}
		osMutexRelease(commonMutex);
	}
	return result;
}
bool GetCopy(char *name, void *data) {
	bool result = false;
	if (osMutexAcquire(commonMutex, osWaitForever) == osOK) {
		for (int i = 0;; ++i) {
			if (cD [ i ].name == NULL) break;
			if (strcmp(name, cD [ i ].name) == 0) {
				memcpy(data, cD [ i ].data, cD [ i ].size);
				result = true;
				break;
			}
		}
		osMutexRelease(commonMutex);
	}
	return result;
}
bool SetCopy(char *name, const void *data) {
	bool result = false;
	if (osMutexAcquire(commonMutex, osWaitForever) == osOK) {
		for (int i = 0;; ++i) {
			if (cD [ i ].name == NULL) break;
			if (strcmp(name, cD [ i ].name) == 0) {
				memcpy(cD [ i ].data, data, cD [ i ].size);
				result = true;
				break;
			}
		}
		osMutexRelease(commonMutex);
	}
	return result;
}
char* GetJsonString(char *name) {
	char *result = NULL;
	if (osMutexAcquire(commonMutex, osWaitForever) == osOK) {
		for (int i = 0;; ++i) {
			if (cD [ i ].name == NULL) break;
			if (strcmp(name, cD [ i ].name) == 0) {
				result = cD [ i ].to_json(cD [ i ].data, cD [ i ].sbuf);
				break;
			}
		}
		osMutexRelease(commonMutex);
	}
	return result;
}
bool SetJsonString(char *name,char* json){
	bool result = false;
	if (osMutexAcquire(commonMutex, osWaitForever) == osOK) {
		for (int i = 0;; ++i) {
			if (cD [ i ].name == NULL) break;
			if (strcmp(name, cD [ i ].name) == 0) {
				cD[i].from_json(json,cD [ i ].data);
				result = true;
				break;
			}
		}
		osMutexRelease(commonMutex);
	}
	return result;
}
