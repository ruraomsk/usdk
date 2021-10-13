/*
 * Commander.c
 *
 *  Created on: 8 окт. 2021 г.
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
typedef struct {
	char *command;
	char* (*doit)(char*);
} give_singl_command;
typedef struct {
	char *command;
	char* (*domit)(char*, int param);
} give_multi_command;

extern YearSet yearSet;
extern WeekSet weekSet;
extern DaySet  daySet;
extern AllPks  allPks;
void correctJson(char *buffer) {
	int count = 0;
	for (;;) {
		if (*buffer == '\0') return;
		if (*buffer == '{') count++;
		if (*buffer == '}') {
			count--;
			if (count == 0) {
				*++buffer = '\0';
				return;
			}
		}
		buffer++;
	}
}

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
char* give_me_SetupDK(char *buffer) {
	return addText(SetupDKName, buffer);
}
char* give_me_Cameras(char *buffer) {
	return addText(CameraSetName, buffer);
}
char* give_me_TCPMain(char *buffer) {
	return addText(TCPSetMainName, buffer);
}
char* give_me_TCPSec(char *buffer) {
	return addText(TCPSetSecName, buffer);
}
char* give_me_OneMonth(char *buffer, int param) {
	*buffer = '\0';
	strcat(buffer, "onemonth");
	strcat(buffer, "=");
	OneMonthToJsonString(&yearSet, param, buffer + strlen(buffer), MAX_LEN_TCP_MESSAGE - strlen(buffer));
	return buffer;
}
char* give_me_OneWeek(char *buffer, int param) {
	*buffer = '\0';
	strcat(buffer, "oneweek");
	strcat(buffer, "=");
	OneWeekToJsonString(&weekSet, param, buffer + strlen(buffer), MAX_LEN_TCP_MESSAGE - strlen(buffer));
	return buffer;
}
char* give_me_OneDay(char *buffer, int param) {
	*buffer = '\0';
	strcat(buffer, "oneday");
	strcat(buffer, "=");
	OneDayToJsonString(&daySet, param, buffer + strlen(buffer), MAX_LEN_TCP_MESSAGE - strlen(buffer));
	return buffer;
}
char* give_me_OnePK(char *buffer, int param) {
	*buffer = '\0';
	strcat(buffer, "onepk");
	strcat(buffer, "=");
	OnePKToJsonString(&allPks, param, buffer + strlen(buffer), MAX_LEN_TCP_MESSAGE - strlen(buffer));
	return buffer;
}


// @formatter:off
// Команды без параметров
give_singl_command cs[]={
				{.command="give-me-SetupDK",&give_me_SetupDK},
				{.command="give-me-Cameras",&give_me_Cameras},
				{.command="give-me-TCPMain",&give_me_TCPMain},
				{.command="give-me-TCPSec",&give_me_TCPSec},
				{.command=NULL,NULL},
};
// Команды с параметрами
give_multi_command cm[]={
				{.command="give-me-OneMonth",&give_me_OneMonth},
				{.command="give-me-OneWeek",&give_me_OneWeek},
				{.command="give-me-OneDay",&give_me_OneDay},
				{.command="give-me-OnePK",&give_me_OnePK},
				{.command=NULL,NULL},
};

// @formatter:on

char* doGiveCommand(char *buffer) {
	for (int i = 0;; ++i) {
		if (cm [ i ].command == NULL) break;
		if (strncmp(buffer, cm [ i ].command, strlen(cm [ i ].command)) == 0) {
			int param;
			sscanf(buffer + strlen(cm [ i ].command), ",%d", &param);
			return cm [ i ].domit(buffer, param);
		}
	}
	for (int j = 0;; ++j) {
		if (cs [ j ].command == NULL) break;
		if (strncmp(buffer, cs [ j ].command, strlen(cs [ j ].command)) == 0) {
			return cs [ j ].doit(buffer);
		}
	}
	return NULL;
}
char* doSetCommand(char *buffer) {
	return NULL;
}

