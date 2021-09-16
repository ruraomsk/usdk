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

PhasesSet fazeSet;
SetupDK setupDK;
YearSet yearSet;
WeekSet weekSet;
DaySet 	daySet;

HeapStats_t pxHeapStats;

void initCommonData(void) {
	vPortGetHeapStats(&pxHeapStats);
//	JSON_Value *temp = FilesGetJson("set/fazes.set");
	char buffer[]="{\"f\":[1,2,3]}";
    JSONStatus_t err=JSON_Validate(buffer, sizeof(buffer)-1);
//	if (temp == NULL) {
		clearPhasesSet(&fazeSet);
//		DeviceLog(SUB_FILES, "Отсутствует настройка фаз! set/fazes.set");
		char *tf = PhasesSetToJsonString(&fazeSet);
//		FilesSetJson("set/fazes.set", temp);
//	} else {
		PhasesSetFromJsonString(tf, &fazeSet);
		vPortFree(tf);
//	}
//	json_value_free(temp);
	vPortGetHeapStats(&pxHeapStats);
//	temp = FilesGetJson("set/setdk.set");
//	if (temp == NULL) {
		clearSetupDK(&setupDK);
//		DeviceLog(SUB_FILES, "Отсутствует настройка ДК! set/setdk.set");
		char *t = SetupDKToJsonString(&setupDK);
		SetupDKFromJsonString(t, &setupDK);
		vPortFree(t);
//	}
//	json_value_free(temp);
	vPortGetHeapStats(&pxHeapStats);
	JSON_Value * temp = FilesGetJson("set/year.set");
	if (temp == NULL) {
		clearYearSet(&yearSet);
		DeviceLog(SUB_FILES, "Отсутствует настройка годового плана! set/year.set");
		temp = YearSetToJsonValue(&yearSet);
		FilesSetJson("set/year.set", temp);
	} else {
		YearSetFromJsonValue(temp, &yearSet);
	}
	json_value_free(temp);
	vPortGetHeapStats(&pxHeapStats);
	temp = FilesGetJson("set/week.set");
	if (temp == NULL) {
		clearWeekSet(&weekSet);
		DeviceLog(SUB_FILES, "Отсутствует настройка недельных планов! set/week.set");
		temp = WeekSetToJsonValue(&weekSet);
		FilesSetJson("set/week.set", temp);
	} else {
		WeekSetFromJsonValue(temp, &weekSet);
	}
	json_value_free(temp);
	vPortGetHeapStats(&pxHeapStats);
	temp = FilesGetJson("set/day.set");
	if (temp == NULL) {
		clearDaySet(&daySet);
		DeviceLog(SUB_FILES, "Отсутствует настройка суточных планов! set/day.set");
		temp = DaySetToJsonValue(&daySet);
		FilesSetJson("set/day.set", temp);
	} else {
		DaySetFromJsonValue(temp, &daySet);
	}
	json_value_free(temp);
	vPortGetHeapStats(&pxHeapStats);

}
int loadCommonDataFromFile(char *path);

int loadSetFazesFromFile(char *path);
int setFazes(char *jsonstring);
