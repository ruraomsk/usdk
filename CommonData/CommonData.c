/*
 * CommonData.c
 *
 *  Created on: 13 сент. 2021 г.
 *      Author: rura
 */

#include "CommonData.h"
#include "Files.h"
#include "DeviceLogger.h"
#include "DebugLogger.h"
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
TimeDevice timeDevice;
GPSSet gpsSet;
CameraSet cameraSet;
AllPks allPks;
NowState nowState;
// @formatter:off
CommonData cD[] = {
		{ .name = PhasesName, .data = (void *)&phasesSet, .size = sizeof(PhasesSet), .sbuf = 1200, .to_json =	&PhasesSetToJsonString, .from_json = &PhasesSetFromJsonString,.by_default=&clearPhasesSet},
		{ .name = SetupDKName, .data = (void *)&setupDK, .size =sizeof(SetupDK), .sbuf = 120, .to_json = &SetupDKToJsonString, .from_json = &SetupDKFromJsonString,.by_default=&clearSetupDK},
		{ .name = YearSetName, .data = (void *)&yearSet, .size = sizeof(YearSet), .sbuf = 1200, .to_json =&YearSetToJsonString, .from_json =&YearSetFromJsonString,.by_default=&clearYearSet },
		{ .name = WeekSetName, .data = (void *)&weekSet, .size = sizeof(WeekSet), .sbuf = 1200, .to_json =&WeekSetToJsonString, .from_json = &WeekSetFromJsonString,.by_default=&clearWeekSet},
		{ .name = DaySetName, .data = (void *)&daySet, .size =sizeof(DaySet), .sbuf = 4800, .to_json = &DaySetToJsonString, .from_json = &DaySetFromJsonString,.by_default=&clearDaySet },
		{ .name = DeviceStatusName, .data = (void *)&deviceStatus, .size =sizeof(DeviceStatus), .sbuf = 1200, .to_json = &DeviceStatusToJsonString, .from_json = &DeviceStatusFromJsonString,.by_default=&clearDeviceStatus},
		{ .name = TCPSetMainName,.data=(void *)&mainTCP,.size=sizeof(TCPSet),.sbuf=80,.to_json=&TCPSetToJsonString,.from_json=&TCPSetFromJsonString,.by_default=&clearTCPSet},
		{ .name = TCPSetSecName,.data=(void *)&secondTCP,.size=sizeof(TCPSet),.sbuf=80,.to_json=&TCPSetToJsonString,.from_json=&TCPSetFromJsonString,.by_default=&clearTCPSet},
		{ .name = TimeDeviceName,.data=(void *)&timeDevice,.size=sizeof(TimeDevice),.sbuf=80,.to_json=&TimeDeviceToJsonString,.from_json=&TimeDeviceFromJsonString,.by_default=&clearTimeDevice},
		{ .name = GPSSetName,.data=(void *)&gpsSet,.size=sizeof(GPSSet),.sbuf=80,.to_json=&GPSSetToJsonString,.from_json=&GPSSetFromJsonString,.by_default=&clearGPSSet},
		{ .name = CameraSetName,.data=(void *)&cameraSet,.size=sizeof(CameraSet),.sbuf=4800,.to_json=&CameraSetToJsonString,.from_json=&CameraSetFromJsonString,.by_default=&clearCameraSet},
		{ .name = NowStateName,.data=(void *)&nowState,.size=sizeof(NowState),.sbuf=1200,.to_json=&NowStateToJsonString,.from_json=&NowStateFromJsonString,.by_default=&clearNowState},
		{ .name = AllPksName,.data=(void *)&allPks,.size=sizeof(AllPks),.sbuf=30000,.to_json=&AllPksToJsonString,.from_json=&AllPksFromJsonString,.by_default=&clearAllPks},
		{ .name =NULL }
};
// @formatter:on
char path [ 60 ];
void initCommonData(void) {
	for (int i = 0;; ++i) {
		if (cD [ i ].name == NULL) break;
		snprintf(path, sizeof(path), "set/%s.set", cD [ i ].name);
		char *json = FilesGetJsonString(path);
		if (json == NULL) {
			DeviceLog(SUB_FILES, "Нет данных %s", path);
			cD [ i ].by_default(cD [ i ].data);
			json = cD [ i ].to_json(cD [ i ].data,NULL, cD [ i ].sbuf);
			if (json != NULL) {
				FilesSetJsonString(path, json);
			} else {
				Debug_Message(LOG_FATAL, "Мало памяти %s",path);
				continue;
			}
		}
		cD [ i ].from_json(json, cD [ i ].data);
		cD [ i ].change = false;
		vPortFree(json);
	}
}
void SaveAllChanged() {
	for (int i = 0;; ++i) {
		if (cD [ i ].name == NULL) break;
		if (!cD [ i ].change) continue;
		snprintf(path, sizeof(path), "set/%s.set", cD [ i ].name);
		char* json = cD [ i ].to_json(cD [ i ].data,NULL, cD [ i ].sbuf);
		if (json != NULL) {
			FilesSetJsonString(path, json);
			cD [ i ].change = false;
			vPortFree(json);
		}
	}

}
int Compare(char *name, void *data) {
	int result = -1;
	for (int i = 0;; ++i) {
		if (cD [ i ].name == NULL) break;
		if (strcmp(name, cD [ i ].name) == 0) {
			result = memcmp(data, cD [ i ].data, cD [ i ].size);
			break;
		}
	}
	return result;
}
bool GetCopy(char *name, void *data) {
	bool result = false;
	for (int i = 0;; ++i) {
		if (cD [ i ].name == NULL) break;
		if (strcmp(name, cD [ i ].name) == 0) {
			memcpy(data, cD [ i ].data, cD [ i ].size);
			result = true;
			break;
		}
	}
	return result;
}
void SetChange(char *name) {
	for (int i = 0;; ++i) {
		if (cD [ i ].name == NULL) break;
		if (strcmp(name, cD [ i ].name) == 0) {
			cD [ i ].change = true;
			break;
		}
	}

}
bool SetCopy(char *name, const void *data) {
	bool result = false;
	for (int i = 0;; ++i) {
		if (cD [ i ].name == NULL) break;
		if (strcmp(name, cD [ i ].name) == 0) {
			memcpy(cD [ i ].data, data, cD [ i ].size);
			cD [ i ].change = true;
			result = true;
			break;
		}
	}
	return result;
}
char* GetJsonString(char *name,char* buffer) {
	char *result = NULL;
	for (int i = 0;; ++i) {
		if (cD [ i ].name == NULL) break;
		if (strcmp(name, cD [ i ].name) == 0) {
			result = cD [ i ].to_json(cD [ i ].data, buffer,cD [ i ].sbuf);
			break;
		}
	}
	return result;
}
bool SetJsonString(char *name, char *json) {
	bool result = false;
	for (int i = 0;; ++i) {
		if (cD [ i ].name == NULL) break;
		if (strcmp(name, cD [ i ].name) == 0) {
			cD [ i ].from_json(json, cD [ i ].data);
			result = true;
			break;
		}
	}
	return result;
}
char* GetNameFile(char* name){
	char* result = NULL;
	for (int i = 0;; ++i) {
		if (cD [ i ].name == NULL) break;
		if (strcmp(name, cD [ i ].name) == 0) {
			snprintf(path, sizeof(path), "set/%s.set", cD [ i ].name);
			result=path;
			break;
		}
	}
	return result;
}
