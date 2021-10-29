/*
 * Tech.h
 *
 *  Created on: Oct 18, 2021
 *      Author: rura
 */

#include "cmsis_os.h"
#include "DeviceTime.h"
#include "CommonData.h"

#ifndef TECHNOLOGY_H_
#define TECHNOLOGY_H_

#define ManageOn		9	//КУ

//Режимы

#define RezimLP			5   //Режим ДК локальное управление
#define RezimKU			8   //Режим ДК координированное управление
#define RezimDU			4   //Режим ДК диспетчерское  управление
#define RezimGR			3   //Режим ДК зеленая улица
#define RezimRU			1   //Режим ДК ручное управление

//Устройство ДК

#define DeviceDK		1	//	1 - ДК
#define DeviceVPU		2	//	2 - ВПУ
#define DeviceIPult		3	//	3 - инженерный пульт (ИП УСДК)
#define DeviceUSDK		4	//	4 - УСДК/ДКА
#define DevicePan		5	//	5 - инженерная панель (ИП ДКА)
#define DeviceIBM6		6	//	6 - система (ЭВМ)
#define DeviceIBM7		7	//	7 - система (ЭВМ)
#define DeviceIBM8		8	// 	8 - система (ЭВМ)
#define DeviceIBM9		9	// 	9 - система (ЭВМ)

//Технологический режим
#define TechBP_CK 	1		// 1 - выбор ПК по времени по суточной карте ВР-СК;
#define TechBP_HK 	2		// 2 - выбор ПК по недельной карте ВР-НК;
#define TechDU_CK 	3		// 3 - выбор ПК по времени по суточной карте, назначенной оператором ДУ-СК;
#define TechDU_HK 	4		// 4 - выбор ПК по недельной карте, назначенной оператором  ДУ-НК;
#define TechDU_PK 	5		// 5 - план по запросу оператора ДУ-ПК;
#define TechLP 		6		// 6 - резервный план (отсутствие точного времени) РП;
#define TechModeIP	7 		// 7 – коррекция привязки с ИП;
#define TechModeSR 	8		// 8 – коррекция привязки с сервера;
#define TechYS_PK 	9		// 9 – выбор ПК по годовой карте;
#define TechXT_PK 	10		// 10 – выбор ПК по ХТ;
#define TechKT_PK 	11		// 11 – выбор ПК по картограмме;
#define TechAntiZ	12  	// 12 – противозаторовое управление.

typedef struct {
	int phase;
	int TTprom;
	int TTmin;
	int TLen;
	int pos;
} OneStep;
typedef struct {
	int nomer;
	PhasesSet phs; 	//Настройки фаз для КДУ
	SetPk lp;		//ЛР для данного КДУ
	osMessageQueueId_t inCommand;
	osMessageQueueId_t outPhase;
} SetupKDU;
typedef struct {
	int nomer;
	PhasesSet *phs; 		//Настройки фаз для УСДК
	AllPks *allpks;		//Все планы координации
	osMessageQueueId_t inCommand;
	bool* isDUDK;
	int pk;				//Назначеный ПК или 0
	int ck;				//Назначеный СК или 0
	int nk;				//Назначеный НК или 0
	int phase;			//Назначеная фаза
} SetupUSDK;

void TechnologyInit();
void TechTask(void *arg);
void dontChange();
void BlindIsChanged();

void KDUWork(void *arg);
void USDKWork(void *arg);

bool isWorkPhase(int phase); //Возвращает истина если это исполняемая фаза с контролем времени

//typedef struct{
//	time_t Start;
//	time_t Stop;
//	int Phase;
//	int num;
//}vvvv;
#endif /* TECHNOLOGY_H_ */
