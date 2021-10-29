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
#include "getset.h"

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
	char* (*dotri)(char*, int dk,int param);
} triple_command;

typedef struct {
	char *command;
	bool (*doit)(char*);
} single_set;
typedef struct {
	char *command;
	bool (*domit)(char*, int param);
} multi_set;
typedef struct {
	char *command;
	bool (*dotri)(char*, int dk,int param);
} triple_set;

typedef struct {
	char* name;
	void* data;
}copyData;
typedef struct{
	char *name;
	bool (*doitc)(char*,char*);
}ctrlCmd;
bool loadData=false;

extern YearSet yearSet;
extern WeekSet weekSet;
extern DaySet  daySet;
extern AllPks  allPks;
extern AllPks  dk2Pks;
extern PhasesSet phasesSet;
extern PhasesSet dk2phasesSet;
YearSet copyyearSet;
WeekSet copyweekSet;
DaySet  copydaySet;
AllPks  copyallPks;
AllPks  copydk2Pks;
PhasesSet copyphasesSet;
PhasesSet copydk2phasesSet;
char* findBegin(char* buffer){
	char* start=buffer;
	for (;;) {
		if (*buffer == '\0') return start;
		if (*buffer == '{') return buffer;
		buffer++;
	}
	return start;
}
// @formatter:off
// Команды без параметров
single_command cs[]={
				{.command="give-me-SetupDK",&give_me_SetupDK},
				{.command="give-me-Cameras",&give_me_Cameras},
				{.command="give-me-TCPMain",&give_me_TCPMain},
				{.command="give-me-TCPSec",&give_me_TCPSec},
				{.command="give-me-Status",&give_me_Status},
				{.command="give-me-GPS",&give_me_GPS},
				{.command="give-me-Error",&give_me_Error},
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
				{.name=DK2PksName,(void *)&copydk2Pks},
				{.name=PhasesName,(void *)&copyphasesSet},
				{.name=DK2PhasesName,(void *)&copydk2phasesSet},
				{.name=NULL,NULL},
};

// @formatter:on

bool changeCmd(char* param,char* buffer){
	if(strncmp(param,"begin",strlen("begin"))==0) {
		for (int i = 0;cd[i].name!=NULL ; ++i) {
			GetCopy(cd[i].name, cd[i].data);
		}
		loadData=true;
		strcpy(buffer,"ready");
		return true;
	}
	if(strncmp(param,"end",strlen("end"))==0) {
		if(!loadData){
			strcpy(buffer,"change-begin need");
			return true;
		}
		if(ControlData(buffer, &copyyearSet, &copyweekSet,&copydaySet,&copyallPks,&copyphasesSet)){
			for (int i = 0;cd[i].name!=NULL ; ++i) {
				SetCopy(cd[i].name, cd[i].data);
			}
			loadData=false;
			SaveAllChanged();
			strcpy(buffer,"allright");
		}
		return true;
	}
	return false;
}
bool setSF(char* param,char* buffer){
	if(strncmp(param,"on",strlen("on"))==0){
		NowState ns;
		GetCopy(NowStateName, &ns);
		ns.comdu.IsReqSFDK1=true;
		ns.comdu.IsReqSFDK2=true;
		SetCopy(NowStateName, &ns);
		strcpy(buffer,"ok");
		return true;
	}
	if(strncmp(param,"off",strlen("off"))==0){
		NowState ns;
		GetCopy(NowStateName, &ns);
		ns.comdu.IsReqSFDK1=false;
		ns.comdu.IsReqSFDK2=false;
		SetCopy(NowStateName, &ns);
		strcpy(buffer,"ok");
		return true;
	}
	return false;
}
bool setPK(char* param,char* buffer){
	int cmd;
	sscanf(param,"%d",&cmd);
	NowState ns;
	GetCopy(NowStateName, &ns);

	SetCopy(NowStateName, &ns);
	strcpy(buffer,"ok");
	return true;
}
bool setCK(char* param,char* buffer){
	int cmd;
	sscanf(param,"%d",&cmd);
	NowState ns;
	GetCopy(NowStateName, &ns);

	SetCopy(NowStateName, &ns);
	strcpy(buffer,"ok");
	return true;
}
bool setNK(char* param,char* buffer){
	int cmd;
	sscanf(param,"%d",&cmd);
	NowState ns;
	GetCopy(NowStateName, &ns);

	SetCopy(NowStateName, &ns);
	strcpy(buffer,"ok");
	return true;
}
bool setDU(char* param,char* buffer){
	int cmd;
	sscanf(param,"%d",&cmd);
	NowState ns;
	GetCopy(NowStateName, &ns);

	SetCopy(NowStateName, &ns);
	strcpy(buffer,"ok");
	return true;
}
bool repeat(char* param,char* buffer){
	return true;
}


// @formatter:off
ctrlCmd cc[]={
			{.name="change-",&changeCmd},
			{.name="set-sf-",&setSF},
			{.name="set-pk-",&setPK},
			{.name="set-ck-",&setCK},
			{.name="set-nk-",&setNK},
			{.name="set-du-",&setDU},
			{.name="repeat again",&repeat},
			{.name=NULL,NULL},
};
// @formatter:on

char giveMe[]="give-me-";
char setYou[]="set-you-";
bool isGiveMeCommand(char *buffer) {
	return (strncmp(buffer,giveMe,strlen(giveMe))==0)?true:false;
}
bool isSetYouCommand(char *buffer) {
	return (strncmp(buffer,setYou,strlen(setYou))==0)?true:false;
}
bool isControlCommand(char *buffer){
	for (int i = 0;cc [ i ].name != NULL; ++i) {
		if (strncmp(buffer, cc [ i ].name, strlen(cc [ i ].name)) == 0) {
			return true;
		}
	}
	return false;
}
bool doControlCommand(char* buffer){
	for (int i = 0;cc [ i ].name != NULL; ++i) {
		if (strncmp(buffer, cc [ i ].name, strlen(cc [ i ].name)) == 0) {
			return cc[i].doitc(buffer+strlen(cc [ i ].name),buffer);
		}
	}
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


