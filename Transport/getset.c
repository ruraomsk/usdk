/*
 * giveset.c
 *
 *  Created on: 19 окт. 2021 г.
 *      Author: rura
 */

#include "CommonData.h"
#include "Common_private.h"
#include "DebugLogger.h"
#include "Files.h"
#include <string.h>
#include "Transport.h"
#include "service.h"
#include <stdio.h>
#include <stdbool.h>

extern YearSet yearSet;
extern WeekSet weekSet;
extern DaySet  daySet;
extern AllPks  allPks;
extern PhasesSet phasesSet;

extern YearSet copyyearSet;
extern WeekSet copyweekSet;
extern DaySet  copydaySet;
extern AllPks  copyallPks;
extern PhasesSet copyphasesSet;


char* addText(char *text, char *buffer) {
//	char* buffer=FilesGetJsonString(GetNameFile(text));
//	if(buffer==NULL) {
//		Debug_Message(LOG_FATAL, "Нет данных %s",text);
//		return NULL;
//	}
	*buffer = '\0';
	strcat(buffer, text);
	strcat(buffer, "=");

	GetJsonString(text, buffer + strlen(buffer));
//	correctJson(buffer);
	return buffer;
}
char* give_me_Status(char *buffer) {
	return addText(StatusSetName, buffer);
}
char* give_me_GPS(char *buffer) {
	return addText(GPSSetName, buffer);
}
char* give_me_Error(char *buffer) {
	return addText(ErrorSetName, buffer);
}
char* give_me_SetupDK(char *buffer) {
	return addText(SetupDKName, buffer);
}
bool set_you_SetupDK(char *buffer) {
	return SetJsonString(SetupDKName, buffer);
}
char* give_me_Cameras(char *buffer) {
	return addText(CameraSetName, buffer);
}
bool set_you_Cameras(char *buffer) {
	return SetJsonString(CameraSetName, buffer);
}
char* give_me_TCPMain(char *buffer) {
	return addText(TCPSetMainName, buffer);
}
bool set_you_TCPMain(char *buffer) {
	return SetJsonString(TCPSetMainName, buffer);
}
char* give_me_TCPSec(char *buffer) {
	return addText(TCPSetSecName, buffer);
}
bool set_you_TCPSec(char *buffer) {
	return SetJsonString(TCPSetSecName, buffer);
}
char* give_me_OneMonth(char *buffer, int param) {
	*buffer = '\0';
	strcat(buffer, "onemonth");
	strcat(buffer, "=");
	OneMonthToJsonString(&yearSet, param, buffer + strlen(buffer), MAX_LEN_TCP_MESSAGE - strlen(buffer));
	return buffer;
}
bool set_you_OneMonth(char *buffer, int param) {
	return OneMonthFromJsonString(buffer,&copyyearSet,param);
}
char* give_me_OneWeek(char *buffer, int param) {
	*buffer = '\0';
	strcat(buffer, "oneweek");
	strcat(buffer, "=");
	OneWeekToJsonString(&weekSet, param, buffer + strlen(buffer), MAX_LEN_TCP_MESSAGE - strlen(buffer));
	return buffer;
}
bool set_you_OneWeek(char *buffer, int param) {
	return OneWeekFromJsonString(buffer,&copyweekSet,param);
}

char* give_me_OneDay(char *buffer, int param) {
	*buffer = '\0';
	strcat(buffer, "oneday");
	strcat(buffer, "=");
	OneDayToJsonString(&daySet, param, buffer + strlen(buffer), MAX_LEN_TCP_MESSAGE - strlen(buffer));
	return buffer;
}
bool set_you_OneDay(char *buffer, int param) {
	return OneDayFromJsonString(buffer,&copydaySet,param);
}
char* give_me_OnePK(char *buffer, int param) {
	*buffer = '\0';
	strcat(buffer, "onepk");
	strcat(buffer, "=");
	OnePKToJsonString(&allPks, param, buffer + strlen(buffer), MAX_LEN_TCP_MESSAGE - strlen(buffer));
	return buffer;
}
bool set_you_OnePK(char *buffer, int param) {
	return OnePKFromJsonString(buffer,&copyallPks,param);
}
char* give_me_OnePhase(char *buffer, int param) {
	*buffer = '\0';
	strcat(buffer, "onephase");
	strcat(buffer, "=");
	OnePhaseToJsonString(&phasesSet, param, buffer + strlen(buffer), MAX_LEN_TCP_MESSAGE - strlen(buffer));
	return buffer;
}
bool set_you_OnePhase(char *buffer, int param) {
	return OnePhaseFromJsonString(buffer,&copyphasesSet,param);
}
