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
#define MAX_WEEKS 	32
#define MAX_DAYS  	12
#define MAX_LINES 	12
#define MAX_PKS   	64
#define MAX_PHASES 	32
#define MAX_STAGES 	64
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

//Один месяц из годового плана
typedef struct {
	uint8_t num;
	uint8_t weeks [ 31 ];
} OneMonth;
//Весь годовой набор
typedef struct {
	OneMonth months [ 12 ];
} YearSet;
//Одна строка недельного плана
typedef struct {
	uint8_t num;
	uint8_t days [ 7 ];
} OneWeek;
//Все недельные планы
typedef struct {
	OneWeek weeks [ MAX_WEEKS ];
} WeekSet;

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
	int tru;
	bool razlen;
	int tc;
	int shift;
	Stage stages [ MAX_STAGES ];
} SetPk;
//Все планы координации устройства
typedef struct {
	SetPk pks [ MAX_PKS ];
} AllPks;


//Настройки устройства
typedef struct {
	int ID;
	bool Ethertnet;
	bool Gprs;
	bool Gps;
	bool Usb;
} DeviceStatus;
//Настройки обмена по TCP
typedef struct {
	char ip [ 20 ];
	int port;
	int tread;
	int twrite;
	int tque;
}TCPSet;
//Настройка временных зон
typedef struct {
	int TimeZone;
	bool Summer;
}TimeDevice;

//Состояние GPS
typedef struct{
	bool Ok;		//	Все исправно
	bool E01;		// 	Нет связи с приемником
	bool E02;		//	Ошибка CRC
	bool E03;		//  Нет валидного времени
	bool E04;		//  Мало спутников
	bool Seek;		//  Поиск спутников после включения
} GPSSet;
// Описание одной подключенной камеры
typedef struct {
	char ip [ 20 ];		//json:"ip"
	int port;			//json:"port"
	int id;				//json:"id"
	char login[20];		//json:"login"
	char password[20];	//json:"password"
	int chanels;
}OneCamera;
// Все Подключенные камеры
typedef struct {
	OneCamera cameras[MAX_CAMERAS];		//json:"cameras"
}CameraSet;

void initCommonData(void);
int getWeekDay(WeekSet *weekSet, int week, int day);
OneDay* getOneDay(DaySet *daySet, int num);
int getYearDay(YearSet *yearSet, int month, int day);
DefinePhase* getPhase(PhasesSet *phasesSet, int num);

int Compare(char *name, void *data);
bool GetCopy(char *name, void *data);
bool SetCopy(char *name, const void *data);
char* GetJsonString(char *name);
bool SetJsonString(char *name,char* json);
bool SetJsonString(char *name,char* json);
#endif /* COMMONDATA_H_ */
