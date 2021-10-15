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
#include <stdbool.h>

typedef struct {
	char *command;
	char* (*doit)(char*);
} single_command;
typedef struct {
	char *command;
	char* (*domit)(char*, int param);
} multi_command;

typedef struct {
	char *command;
	bool (*doit)(char*);
} single_set;
typedef struct {
	char *command;
	bool (*domit)(char*, int param);
} multi_set;
typedef struct {
	char* name;
	void* data;
}copyData;
bool loadData=false;

extern YearSet yearSet;
extern WeekSet weekSet;
extern DaySet  daySet;
extern AllPks  allPks;
extern PhasesSet phasesSet;
YearSet copyyearSet;
WeekSet copyweekSet;
DaySet  copydaySet;
AllPks  copyallPks;
PhasesSet copyphasesSet;
char* findBegin(char* buffer){
	char* start=buffer;
	for (;;) {
		if (*buffer == '\0') return start;
		if (*buffer == '{') return buffer;
		buffer++;
	}
	return start;
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



// @formatter:off
// Команды без параметров
single_command cs[]={
				{.command="give-me-SetupDK",&give_me_SetupDK},
				{.command="give-me-Cameras",&give_me_Cameras},
				{.command="give-me-TCPMain",&give_me_TCPMain},
				{.command="give-me-TCPSec",&give_me_TCPSec},
				{.command=NULL,NULL},
};
single_set ss[]={
				{.command="set-you-SetupDK=",&set_you_SetupDK},
				{.command="set-you-Cameras=",&set_you_Cameras},
				{.command="set-you-TCPMain=",&set_you_TCPMain},
				{.command="set-you-TCPSec=",&set_you_TCPSec},
				{.command=NULL,NULL},
};
// Команды с параметрами
multi_command cm[]={
				{.command="give-me-OneMonth",&give_me_OneMonth},
				{.command="give-me-OneWeek",&give_me_OneWeek},
				{.command="give-me-OneDay",&give_me_OneDay},
				{.command="give-me-OnePK",&give_me_OnePK},
				{.command="give-me-OnePhase",&give_me_OnePhase},
				{.command=NULL,NULL},
};
multi_set sm[]={
				{.command="set-you-OneMonth",&set_you_OneMonth},
				{.command="set-you-OneWeek",&set_you_OneWeek},
				{.command="set-you-OneDay",&set_you_OneDay},
				{.command="set-you-OnePK",&set_you_OnePK},
				{.command="set-you-OnePhase",&set_you_OnePhase},
				{.command=NULL,NULL},
};
copyData cd[]={
		{.name=YearSetName,(void *)&copyyearSet},
		{.name=WeekSetName,(void *)&copyweekSet},
		{.name=DaySetName,(void *)&copydaySet},
		{.name=AllPksName,(void *)&copyallPks},
		{.name=PhasesName,(void *)&copyphasesSet},
		{.name=NULL,NULL},
};
// @formatter:on

char giveMe[]="give-me-";
char setYou[]="set-you-";
char beginChange[]="change-begin";
char endChange[]="change-end";
char repeatAgain[]="repeat again";
bool isGiveMeCommand(char *buffer) {
	return (strncmp(buffer,giveMe,strlen(giveMe))==0)?true:false;
}
bool isSetYouCommand(char *buffer) {
	return (strncmp(buffer,setYou,strlen(setYou))==0)?true:false;
}
bool isControlCommand(char *buffer){
	if(strncmp(buffer,beginChange,strlen(beginChange))==0) return true;
	if(strncmp(buffer,endChange,strlen(endChange))==0) return true;
	if(strncmp(buffer,repeatAgain,strlen(repeatAgain))==0) return true;
	return false;
}
bool doControlCommand(char* buffer){
	if(strncmp(buffer,beginChange,strlen(beginChange))==0) {
		for (int i = 0;cd[i].name!=NULL ; ++i) {
			GetCopy(cd[i].name, cd[i].data);
		}
		loadData=true;
		strcpy(buffer,"ready");
		return true;
	}
	if(strncmp(buffer,endChange,strlen(endChange))==0) {
		if(!loadData){
			strcpy(buffer,"change-begin need");
			return true;
		}
		if(true){
			ControlData(buffer, &copyyearSet, &copyweekSet,&copydaySet,&copyallPks,&copyphasesSet);
			for (int i = 0;cd[i].name!=NULL ; ++i) {
				SetCopy(cd[i].name, cd[i].data);
			}
			loadData=false;
			strcpy(buffer,"allright");
		}
		return true;
	}
	if(strncmp(buffer,repeatAgain,strlen(repeatAgain))==0) return true;
	return false;
}
char* doGiveCommand(char *buffer) {
	for (int i = 0;cm [ i ].command != NULL; ++i) {
		if (strncmp(buffer, cm [ i ].command, strlen(cm [ i ].command)) == 0) {
			int param;
			sscanf(buffer + strlen(cm [ i ].command), ",%d", &param);
			return cm [ i ].domit(buffer, param);
		}
	}
	for (int j = 0;cs [ j ].command != NULL; ++j) {
		if (strncmp(buffer, cs [ j ].command, strlen(cs [ j ].command)) == 0) {
			return cs [ j ].doit(buffer);
		}
	}
	return NULL;
}
bool doSetCommand(char *buffer) {
	for (int j = 0;ss [ j ].command != NULL; ++j) {
		if (strncmp(buffer, ss [ j ].command, strlen(ss [ j ].command)) == 0) {

			return ss [ j ].doit(buffer+strlen(ss [ j ].command)+1);
		}
	}
	if(!loadData) return false;
	for (int i = 0;sm [ i ].command != NULL; ++i) {
		if (strncmp(buffer, sm [ i ].command, strlen(sm [ i ].command)) == 0) {
			int param;
			sscanf(buffer + strlen(sm [ i ].command), ",%d=", &param);
			char* newbuff=findBegin(buffer);
			return sm [ i ].domit(newbuff, param);
		}
	}
	return false;
}


