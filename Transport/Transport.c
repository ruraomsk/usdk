/*
 * Transport.c
 *
 *  Created on: 30 авг. 2021 г.
 *      Author: rura
 */

#include "cmsis_os.h"

#include "DeviceLogger.h"
#include "DebugLogger.h"
#include "Transport.h"
#include "CommonData.h"
#include "Camera.h"
//DeviceStatus deviceStatus;

bool GPRSNeed = false;			//Готовность работы GPRS если есть
bool ToServerTCPStart = false;
bool ToServerGPRSStart = false;
bool TCPError = false;
bool GPRSError = false;


typedef StaticTask_t osStaticThreadDef_t;

/* Definitions for ToServerTCP */
osThreadId_t ToServerTCPHandle;
const osThreadAttr_t ToServerTCP_attributes = { .name = "ToTCP", .stack_size = 2048 * 4, .priority =
		(osPriority_t) osPriorityRealtime, };
/* Definitions for ToServerGPRS */

/* Definitions for FromServerTCP */
osThreadId_t FromServerTCPHandle;
const osThreadAttr_t FromServerTCP_attributes = { .name = "FromTCP", .stack_size = 2048 * 4, .priority =
		(osPriority_t) osPriorityRealtime, };
/* Definitions for ToServerGPRS */
osThreadId_t ToServerGPRSHandle;
const osThreadAttr_t ToServerGPRS_attributes = { .name = "ToGPRS", .stack_size = 2048 * 4, .priority =
		(osPriority_t) osPriorityRealtime, };
/* Definitions for FromServerGPRS */
osThreadId_t FromServerGPRSHandle;
const osThreadAttr_t FromServerGPRS_attributes = { .name = "FromGPRS", .stack_size = 2048 * 4, .priority =
		(osPriority_t) osPriorityRealtime, };

extern bool mainConnect;

DeviceStatus devStatus;

void startGPRS() {
	if (!devStatus.Gprs) return;
	Debug_Message(LOG_INFO, "Переключились с ETH на GPRS");
	setGPRSNeed(true);
}
void stopGPRS() {
	if (!devStatus.Gprs) return;
	Debug_Message(LOG_INFO, "Переключились с GPRS на ETH");
	setGoodGPRS(false);
	setGPRSNeed(false);

}
void noETHandGPRS() {
	setGoodGPRS(false);
	setGoodTCP(false);
	Debug_Message(LOG_INFO, "Нет устройств связи");
}
void StartToServerTCP(void *argument) {
	while (1) {
		while (!ToServerTCPStart ) {
			osDelay(STEP_CONTROL);
		}
//		Debug_Message(LOG_INFO, "Запускаем ToServerTCP");
		ToServerTCPLoop();
//		Debug_Message(LOG_INFO, "остановился ToServerTCP");
		osDelay(STEP_TCP);

	}
}
void StartFromServerTCP(void *argument) {
	while (1) {
//		Debug_Message(LOG_INFO, "Запускаем FromServerTCP %d %d",osThreadGetStackSpace(FromServerTCPHandle),osThreadGetStackSpace(ToServerTCPHandle));
		FromServerTCPLoop();
		setToServerTCPStart(false);
		setGoodTCP(false);
//		Debug_Message(LOG_INFO, "остановился FromServerTCP %d %d",osThreadGetStackSpace(FromServerTCPHandle),osThreadGetStackSpace(ToServerTCPHandle));
		osDelay(STEP_TCP);

	}
}
void StartToServerGPRS(void *argument) {
	while (1) {
		while (!(ToServerGPRSStart )) {
			osDelay(STEP_GPRS);
		}
//		Debug_Message(LOG_INFO, "Запускаем ToServerGPRS");
		ToServerGPRSLoop();
		setGoodGPRS(false);
//		Debug_Message(LOG_INFO, "остановился ToServerGPRS");
		osDelay(STEP_GPRS);
	}
}

void StartFromServerGPRS(void *argument) {
	while (1) {
		while (!GPRSNeed || isGoodTCP()) {
			osDelay(STEP_GPRS);
		}
		osDelay(STEP_GPRS);
		Debug_Message(LOG_INFO, "Запускаем FromServerGPRS");
		FromServerGPRSLoop();
		setToServerGPRSStart(false);
		setGoodGPRS(false);
		Debug_Message(LOG_INFO, "остановился FromServerGPRS");
	}
}

void TransportStart(void) {


	//0 - Переход от GPRS на Ethernet
	//1 - Переход от Ethernet на GPRS
	//-1 - Нет связи с внешним миром
// Временно для отладки
//	osDelay(1000U);
	TCPSet mainTCP, secTCP;
	GetCopy(TCPSetMainName, &mainTCP);
	mainTCP.tread = 10;
	mainTCP.twrite = 10;
	mainTCP.tque = 300;
	SetCopy(TCPSetMainName, &mainTCP);
	GetCopy(TCPSetSecName, &secTCP);
	secTCP.tread = 10;
	secTCP.twrite = 10;
	secTCP.tque = 10;
	secTCP.port = 2094;
	SetCopy(TCPSetSecName, &secTCP);
	setTimeoutForChanel(20);
	GetCopy(DeviceStatusName, &devStatus);
	if (!devStatus.Ethertnet && devStatus.Gprs) {setGPRSNeed(true);setGoodTCP(false);}
	if (devStatus.Ethertnet && !devStatus.Gprs) {setGPRSNeed(false);};

	if (devStatus.Ethertnet) {
		setGoodTCP(false);
		/* creation of FromServerTCP */
		FromServerTCPHandle = osThreadNew(StartFromServerTCP, NULL, &FromServerTCP_attributes);

		/* creation of ToServerTCP */
		ToServerTCPHandle = osThreadNew(StartToServerTCP, NULL, &ToServerTCP_attributes);
	}
	if (devStatus.Gprs) {
		setGoodGPRS(false);
		setGPRSNeed(true);
		/* creation of FromServerGPRS */
		FromServerGPRSHandle = osThreadNew(StartFromServerGPRS, NULL, &FromServerGPRS_attributes);

		/* creation of ToServerGPRS */
		ToServerGPRSHandle = osThreadNew(StartToServerGPRS, NULL, &ToServerGPRS_attributes);
	}
	if (!devStatus.Ethertnet && !devStatus.Gprs) {
		noETHandGPRS();
	}
	return;
}

