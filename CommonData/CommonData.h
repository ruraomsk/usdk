/*
 * CommonData.h
 *
 *  Created on: 13 сент. 2021 г.
 *      Author: rura
 */

#ifndef COMMONDATA_H_
#define COMMONDATA_H_

#include "parson.h"

typedef enum {
	false=0,
	true,
} boolean;

#define MAX_WEEKS 	32
#define MAX_DAYS  	32
#define MAX_LINES 	64
#define MAX_PKS   	64
#define MAX_PHASES 	32
#define MAX_STAGES 	64
//Описание одной фазы со всеми нужными вещами
typedef struct {
	int num;
	boolean work;
} DefinePhase;
//Все фазы устройства
typedef struct{
	DefinePhase defPhase[MAX_PHASES];
}PhasesSet;
//Настройки ДК
typedef struct{
	int dkn;
	int tmaxf;
	int tminf;
	int tminmax;
	int dktype;
	int extn;
	int tprom;
	boolean preset;
} SetupDK;
//Один месяц из годового плана
typedef struct {
	int num;
	int weeks[31];
} OneMonth;
//Весь годовой набор
typedef struct {
	OneMonth months[12];
} YearSet;
//Одна строка недельного плана
typedef struct {
	int num;
	int days[7];
} OneWeek;
//Все недельные планы
typedef struct {
	OneWeek weeks[MAX_WEEKS];
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
	Line lines[MAX_LINES];
}OneDay;
//Все суточные планы
typedef struct {
	OneDay days[MAX_DAYS];
}DaySet;
//Одна строка плана координации
typedef struct {
	int line;
	int start;
	int stop;
	int num;
	int tf;
	boolean plus;
	boolean trs;
	int dt;
}Stage;
//Один план координации
typedef struct {
	int pk;
	int tru;
	boolean razlen;
	int tc;
	int shift;
	Stage stages[MAX_STAGES];
}SetPk;
//Все планы координации устройства
typedef struct {
	SetPk pks[MAX_PKS];
}AllPks;
void initCommonData(void);

void clearSetupDK(SetupDK *setupDK);
char *SetupDKToJsonString(SetupDK *setupDK);
void SetupDKFromJsonString(char* root, SetupDK *setupDK);

void clearWeekSet(WeekSet *weekSet);
int getWeekDay(WeekSet *weekSet, int week, int day);

void clearDaySet(DaySet *daySet);
OneDay* getOneDay(DaySet *daySet, int num);
char* DaySetToJsonString(DaySet *daySet);
void DaySetFromJsonString(char *root, DaySet *daySet)

void clearYearSet(YearSet *yearSet);
int getYearDay(YearSet *yearSet, int month,int day);


void clearPhasesSet(PhasesSet* phasesSet);
DefinePhase* getPhase(PhasesSet *phasesSet, int num);
char* PhasesSetToJsonString(PhasesSet *phasesSet);
void PhasesSetFromJsonString(char *root, PhasesSet *phasesSet);


#endif /* COMMONDATA_H_ */
