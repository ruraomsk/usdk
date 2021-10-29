/*
 * USDK.c
 *
 *  Created on: 27 окт. 2021 г.
 *      Author: rura
 */
#include "Technology.h"
#include "DebugLogger.h"
#include "CommonData.h"
#include "Common_private.h"
#include <stdio.h>
#include <string.h>

/*
 * Управление КДУ
 *	RDK int `json:"rdk"` //Режим ДК
	// 1 2 Ручное управление
	// 3 Зеленая улица
	// 4 Диспетчерское управление
	// 5 6 Локальное управление
	// 8 9 Координированное управление
	FDK int `json:"fdk"` //Фаза ДК
	// от 1 до 8 номера рабочих фаз
	// 9 промежуточный такт
	// 10 желтое мигание
	// 11 отключен светофор
	// 12 кругом краснный
	DDK int `json:"ddk"` //Устройство ДК
	//	1 - ДК
	//	2 - ВПУ
	//	3 - инженерный пульт (ИП УСДК)
	//	4 - УСДК/ДКА
	//	5 - инженерная панель (ИП ДКА)
	//	6 - система (ЭВМ)
	//	7 - система (ЭВМ)
	// 	8 - система (ЭВМ)
	// 	9 - система (ЭВМ)
	EDK   int  `json:"edk"`   //Неисправность ДК
	PDK   bool `json:"pdk"`   //Признак переходного периода ДК
	EEDK  int  `json:"eedk"`  //дополнительный код неисправности
	ODK   bool `json:"odk"`   //Открыта дверь ДК
	LDK   int  `json:"ldk"`   //Номер фазы на которой сгорели лампы
	FTUDK int  `json:"ftudk"` //Фаза ТУ ДК на момент передачи
	TDK   int  `json:"tdk"`   //Время отработки ТУ в секундах
	FTSDK int  `json:"ftsdk"` //Фаза ТС ДК
	TTCDK int  `json:"ttcdk"` //Время от начала фазы ТС в секундах

 *
 */
extern NowState nowState;
// @formatter:off
SetPk lp={.stages={
							{.line=1,.start=0,.stop=30,.num=1},
							{.line=2,.start=30,.stop=60,.num=2},
							{.line=3,.start=60,.stop=90,.num=3},
							{.line=-1}}};
// @formatter:on

void makeKDU(SetupUSDK* susdk,SetupKDU* skdu){
	skdu->nomer=susdk->nomer;
	skdu->inCommand=osMessageQueueNew(10, sizeof(int), NULL);
	skdu->outPhase=osMessageQueueNew(10, sizeof(int), NULL);
	clearPhasesSet(&skdu->phs);
	memset(&skdu->lp,0,sizeof(SetPk));
	for (int i = 0; lp.stages[i].line!=-1; ++i) {
		skdu->lp.stages[i]=lp.stages[i];
		skdu->lp.tc=lp.stages[i].stop;
	}


}
void USDKWork(void* arg){
	SetupUSDK* susdk=(SetupUSDK*)arg;
	SetupKDU   skdu;
	OneStep    os;
	int oldState=nowState.dk[susdk->nomer].rdk;
	char name[10];
	void makeStep(){
		if (os.TTmin) os.TTmin--;
		if (os.TLen) os.TLen--;
	}
	memset(&os,0,sizeof(OneStep));
	snprintf(name,9,"KDU%d",susdk->nomer);
	makeKDU(susdk, &skdu);
	osThreadAttr_t KDU_attributes = {  .stack_size = 2048 * 4, .priority =(osPriority_t) osPriorityRealtime, };
	KDU_attributes.name=name;
	osThreadNew(KDUWork, &skdu, &KDU_attributes);
	if (nowState.dk[susdk->nomer].rdk==0){
		nowState.dk[susdk->nomer].rdk=RezimLP;
		oldState=nowState.dk[susdk->nomer].rdk;
		nowState.dk[susdk->nomer].ftudk=LocalPlane;
	}
	nowState.dk[susdk->nomer].ddk=DeviceUSDK;
	nowState.dk[susdk->nomer].ttcdk=0;
	nowState.dk[susdk->nomer].ftsdk=0;
	for(;;){
		osDelay(1000U);
		makeStep();
		if (susdk->isDUDK){
			if (isWorkPhase(nowState.comdu.phase) && os.TLen==0){

			}

		}
		nowState.dk[susdk->nomer].ttcdk++;
		nowState.dk[susdk->nomer].ftsdk++;
		while (osMessageQueueGetCount(skdu.outPhase)!=0){
			int phase;
			osMessageQueueGet(skdu.outPhase,&phase,0,osWaitForever);
			if (phase==PromTakt && phase!=nowState.dk[susdk->nomer].fdk){
				Debug_Message(LOG_INFO, "KDU%d phase %d time %d",susdk->nomer,nowState.dk[susdk->nomer].fdk,nowState.dk[susdk->nomer].ftsdk);
				//Начало новой фазы с промтакта
				nowState.dk[susdk->nomer].fdk=phase;
				nowState.dk[susdk->nomer].ftsdk=0;
				Debug_Message(LOG_INFO, "KDU%d phase %d time %d",susdk->nomer,nowState.dk[susdk->nomer].fdk,nowState.dk[susdk->nomer].ftsdk);
			}
			if (phase!=PromTakt && phase!=nowState.dk[susdk->nomer].fdk){
				//Продолжаем считать фазу
				nowState.dk[susdk->nomer].fdk=phase;
				Debug_Message(LOG_INFO, "KDU%d phase %d time %d",susdk->nomer,nowState.dk[susdk->nomer].fdk,nowState.dk[susdk->nomer].ftsdk);
			}

		}

	}

}
