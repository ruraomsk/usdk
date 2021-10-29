/*
 * Tech.c
 *
 *  Created on: Oct 18, 2021
 *      Author: rura
 */
#include "DebugLogger.h"
#include "DeviceLogger.h"
#include "CommonData.h"
#include "Technology.h"

extern DeviceStatus deviceStatus;

extern AllPks allPks;
extern PhasesSet phasesSet;
extern AllPks dk2Pks;
extern PhasesSet dk2phasesSet;
extern NowState nowState;

osThreadId_t TechHandle;
const osThreadAttr_t Tech_attributes = { .name = "Tech", .stack_size = 2048 * 4, .priority =
		(osPriority_t) osPriorityRealtime, };

osThreadId_t USDK1Handle;
const osThreadAttr_t USDK1_attributes = { .name = "USDK1", .stack_size = 2048 * 4, .priority =
		(osPriority_t) osPriorityRealtime, };
osThreadId_t USDK2Handle;
const osThreadAttr_t USDK2_attributes = { .name = "USDK2", .stack_size = 2048 * 4, .priority =
		(osPriority_t) osPriorityRealtime, };
SetupUSDK usdk1,usdk2;

void dontChange(){
	//Запрещает контроллеру принимать любые измения в управлении
	Debug_Message(LOG_INFO, "Запрещено менять ПК");
}

void BlindIsChanged(){
	//Извещение что изменились привязки и при необходимости принять меры
	Debug_Message(LOG_INFO, "Проверить изменилось ли!!");
}
void makeUSDK(int nomer,SetupUSDK* usdk){
	usdk->nomer=nomer;
	if(nomer==0){
		usdk->allpks=&allPks;
		usdk->phs=&phasesSet;
		usdk->isDUDK=&nowState.comdu.IsDUDK1;

	} else {
		usdk->allpks=&dk2Pks;
		usdk->phs=&dk2phasesSet;
		usdk->isDUDK=&nowState.comdu.IsDUDK2;
	}
	usdk->inCommand=osMessageQueueNew(10, sizeof(int), NULL);
}
void TechnologyInit(){
	if (deviceStatus.KDU1){
		makeUSDK(0,&usdk1);
		USDK1Handle = osThreadNew(USDKWork, &usdk1, &USDK1_attributes);
	}
	if (deviceStatus.KDU2){
		makeUSDK(1,&usdk2);
		USDK2Handle = osThreadNew(USDKWork, &usdk2, &USDK2_attributes);
	}

	TechHandle = osThreadNew(TechTask, NULL, &Tech_attributes);
}
void TechTask(void* arg){
	if(nowState.tech==0){
		nowState.tech=TechLP;
	}
	for(;;){
		osDelay(1000U);
	}
}




	//Включились! Первым делом переходим в безопасный режим
	//Проверим все таблицы на полноту
	//
	//Выбираем карты и план координации
	//План координации передаем на исполнение
	//Ставим таймер на время смены ПК по суточной карте если он приходится точно на конец суток то его не взводим
	//Ставим таймер на конец суток
	//Если сработал по смене ПК в суточной карте
	//Не зависимо от изменения номера ПК Расчитать план перехода если план перехода пустой то пропускаем передачу плана перехода
	//и передать его и новый ПК на исполнение
	//Ставим таймер на время смены ПК по суточной карте если он приходится точно на конец суток то его не взводим
	//Если сработал переход на новые сутки то
	//Выбираем карты и план координации
	//Не зависимо от изменения номера ПК Расчитать план перехода если план перехода пустой то пропускаем передачу плана перехода
	//и передать его и новый ПК на исполнение
    //Исполнение команд Диспетчера
	//Останавливаем все таймеры
	//Изменение номера ПК
	//Изменение суточной карты
	//Изменение недельной карты
	//Изменение фазы
	//Отключаем все таймеры взводим таймер контроля повторения команд
	//Останавливаем текущий план и передаем команду на смену фазы
	//Как только срабатывает таймер отключаем повтор фазы
