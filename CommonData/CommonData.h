/*
 * CommonData.h
 *
 *  Created on: 13 сент. 2021 г.
 *      Author: rura
 */

#ifndef COMMONDATA_H_
#define COMMONDATA_H_

#include <stdint.h>
#include <stdbool.h>
#include "CommonExt.h"

#define MAX_WEEKS 	32
#define MAX_DAYS  	12
#define MAX_LINES 	12
#define MAX_PKS   	24
#define MAX_PHASES 	24
#define MAX_STAGES 	24
#define MAX_CAMERAS 16


//Описание одной фазы со всеми нужными вещами
typedef struct {
	int num;
	bool work;
} DefinePhase;
//Все фазы устройства
typedef struct {
	DefinePhase defPhase [ MAX_PHASES ];
} PhasesSet;
#define PhasesName "phases"

//Настройки ДК
typedef struct {
	int dkn;
	int tmaxf;
	int tminf;
	int tminmax;
	int dktype;
	int extn;
	int tprom;
	bool preset;
} SetupDK;
#define SetupDKName "setdk"

//Один месяц из годового плана
typedef struct {
	uint8_t num;
	uint8_t weeks [ 31 ];
} OneMonth;
//Весь годовой набор
typedef struct {
	OneMonth months [ 12 ];
} YearSet;
#define YearSetName "year"
//Одна строка недельного плана
typedef struct {
	uint8_t num;
	uint8_t days [ 7 ];
} OneWeek;
//Все недельные планы
typedef struct {
	OneWeek weeks [ MAX_WEEKS ];
} WeekSet;
#define WeekSetName "week"
//Одна строка суточного плана
typedef struct {
	int time;
	int npk;
} Line;
//Весь один суточный план
typedef struct {
	int num;
	int count;
	Line lines [ MAX_LINES ];
} OneDay;
//Все суточные планы
typedef struct {
	OneDay days [ MAX_DAYS ];
} DaySet;
#define DaySetName "day"
//Одна строка плана координации
typedef struct {
	int line;
	int start;
	int stop;
	int num;
	int tf;
	bool plus;
	bool trs;
	int dt;
} Stage;
//Один план координации
typedef struct {
	int pk;
	int tpu;
	bool razlen;
	int tc;
	int shift;
	bool twot;
	Stage stages [ MAX_STAGES ];
} SetPk;
//Все планы координации устройства
typedef struct {
	SetPk pks [ MAX_PKS ];
} AllPks;
#define AllPksName "pks"

//Настройки устройства
typedef struct {
	int ID;
	bool Ethertnet;
	bool Gprs;
	bool Gps;
	bool Usb;
	bool Camera;
} DeviceStatus;
#define DeviceStatusName "devst"
//Настройки обмена по TCP
typedef struct {
	char ip [ 20 ];
	int port;
	int tread;
	int twrite;
	int tque;
}TCPSet;
#define TCPSetMainName "cmain"
#define TCPSetSecName "csec"

//Настройка временных зон
typedef struct {
	int TimeZone;
	bool Summer;
}TimeDevice;
#define TimeDeviceName "tdev"
// Описание одной подключенной камеры
typedef struct {
	char ip [ 20 ];		//json:"ip"
	int port;			//json:"port"
	int id;				//json:"id"
	char login[20];		//json:"login"
	char password[20];	//json:"password"
	int chanels;
}OneCameraConn;
// Все Подключенные камеры
typedef struct {
	OneCameraConn cameras[MAX_CAMERAS];		//json:"cameras"
}CameraSet;
#define CameraSetName "cam"
void initCommonData(void);
void SaveAllChanged(void);

int getWeekDay(WeekSet *weekSet, int week, int day);
OneDay* getOneDay(DaySet *daySet, int num);
int getYearDay(YearSet *yearSet, int month, int day);
DefinePhase* getPhase(PhasesSet *phasesSet, int num);

int Compare(char *name, void *data);
bool GetCopy(char *name, void *data);
bool SetCopy(char *name, const void *data);
char* GetJsonString(char *name,char* buffer);
bool SetJsonString(char *name,char* json);
char* GetNameFile(char* name);
#endif /* COMMONDATA_H_ */
