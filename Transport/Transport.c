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
//DeviceStatus deviceStatus;

bool GPRSNeed = false;			//Готовность работы GPRS если есть
bool ToServerTCPStart = false;
bool ToServerGPRSStart = false;
bool TCPError = false;
bool GPRSError = false;

//Очереди для Ethernet
osMessageQueueId_t ETHToServerQueue;
osMessageQueueId_t ETHFromServerQueue;
osMessageQueueId_t ETHToServerSecQueue;
osMessageQueueId_t ETHFromServerSecQueue;

//Очереди для GPRS
osMessageQueueId_t GPRSToServerQueue;
osMessageQueueId_t GPRSFromServerQueue;
osMessageQueueId_t GPRSToServerSecQueue;
osMessageQueueId_t GPRSFromServerSecQueue;

//Очереди от основной программы управления
osMessageQueueId_t ChangeStatus;
osMessageQueueId_t ToServerQueue;
osMessageQueueId_t FromServerQueue;
osMessageQueueId_t ToServerSecQueue;
osMessageQueueId_t FromServerSecQueue;

osMutexId_t TransportMutex;
/* Definitions for ToServerTCP */
osThreadId_t ToServerTCPHandle;
const osThreadAttr_t ToServerTCP_attributes = { .name = "ToTCP", .stack_size = 1024 * 4, .priority =
		(osPriority_t) osPriorityLow, };
/* Definitions for FromServerTCP */
osThreadId_t FromServerTCPHandle;
const osThreadAttr_t FromServerTCP_attributes = { .name = "FromTCP", .stack_size = 1024 * 4, .priority =
		(osPriority_t) osPriorityLow, };
/* Definitions for ToServerGPRS */
osThreadId_t ToServerGPRSHandle;
const osThreadAttr_t ToServerGPRS_attributes = { .name = "ToGPRS", .stack_size = 1024 * 4, .priority =
		(osPriority_t) osPriorityLow, };
/* Definitions for FromServerGPRS */
osThreadId_t FromServerGPRSHandle;
const osThreadAttr_t FromServerGPRS_attributes = { .name = "FromGPRS", .stack_size = 1024 * 4, .priority =
		(osPriority_t) osPriorityLow, };

osThreadId_t TechExchHandle;
const osThreadAttr_t TechExch_attributes = { .name = "TechExch", .stack_size = 1024 * 4, .priority =
		(osPriority_t) osPriorityLow, };

#define tout 10
extern bool mainConnect;

DeviceStatus devStatus;

void startGPRS() {
	int c = 1;
	if (!devStatus.Gprs) return;
	osMessageQueuePut(ChangeStatus, &c, 0, 0);
	Debug_Message(LOG_INFO, "Переключились с ETH на GPRS");
	setGPRSNeed(true);
}
void stopGPRS() {
	int c = 0;
	if (!devStatus.Gprs) return;
	osMessageQueuePut(ChangeStatus, &c, 0, 0);
	Debug_Message(LOG_INFO, "Переключились с GPRS на ETH");
	setGoodGPRS(false);
	setGPRSNeed(false);

}
void noETHandGPRS() {
	MessageFromQueue msg;
	int c = -1;
	setGoodGPRS(false);
	setGoodTCP(false);
	osMessageQueuePut(ChangeStatus, &c, 0, 0);
	Debug_Message(LOG_INFO, "Нет устройств связи");
	for (;;) {
		if (osMessageQueueGet(ToServerQueue, &msg, NULL, tout) == osOK) {
			DeviceLog(SUB_TRANSPORT, "Пришло сообщение на главный ");
			vPortFree(msg.message);
		}
		if (osMessageQueueGet(ToServerSecQueue, &msg, NULL, tout) == osOK) {
			DeviceLog(SUB_TRANSPORT, "Пришло сообщение на второй ");
			vPortFree(msg.message);
		}
	}
}
void StartToServerTCP(void *argument) {
	while (1) {
		while (!(ToServerTCPStart && mainConnect)) {
			osDelay(STEP_CONTROL);
		}
		Debug_Message(LOG_INFO, "Запускаем ToServerTCP");
		ToServerTCPLoop();
		setToServerTCPStart(false);
		setGoodTCP(false);
		Debug_Message(LOG_INFO, "остановился ToServerTCP");
		osDelay(STEP_TCP);

	}
}
void StartFromServerTCP(void *argument) {
	while (1) {
		Debug_Message(LOG_INFO, "Запускаем FromServerTCP");
		FromServerTCPLoop();
		setToServerTCPStart(false);
		setGoodTCP(false);
		Debug_Message(LOG_INFO, "остановился FromServerTCP");
		osDelay(STEP_TCP);

	}
}
void StartToServerGPRS(void *argument) {
	while (1) {
		while (!(ToServerGPRSStart && mainConnect)) {
			osDelay(STEP_CONTROL);
		}
		Debug_Message(LOG_INFO, "Запускаем ToServerGPRS");
		ToServerGPRSLoop();
		setGoodGPRS(false);
		Debug_Message(LOG_INFO, "остановился ToServerGPRS");
		osDelay(STEP_GPRS);
	}
}

void StartFromServerGPRS(void *argument) {
	while (1) {
		while (!GPRSNeed) {
			osDelay(STEP_CONTROL);
		}
		Debug_Message(LOG_INFO, "Запускаем FromServerGPRS");
		FromServerGPRSLoop();
		setToServerGPRSStart(false);
		setGoodGPRS(false);
		Debug_Message(LOG_INFO, "остановился FromServerGPRS");
		osDelay(STEP_GPRS);

	}
}

void mainTransportLoop(void) {
	TransportMutex = osMutexNew(NULL);
	ETHToServerQueue = osMessageQueueNew(6, sizeof(MessageFromQueue), NULL);
	ETHFromServerQueue = osMessageQueueNew(6, sizeof(MessageFromQueue), NULL);
	ETHToServerSecQueue = osMessageQueueNew(6, sizeof(MessageFromQueue), NULL);
	ETHFromServerSecQueue = osMessageQueueNew(6, sizeof(MessageFromQueue), NULL);

	GPRSToServerQueue = osMessageQueueNew(6, sizeof(MessageFromQueue), NULL);
	GPRSFromServerQueue = osMessageQueueNew(6, sizeof(MessageFromQueue), NULL);
	GPRSToServerSecQueue = osMessageQueueNew(6, sizeof(MessageFromQueue),
	NULL);
	GPRSFromServerSecQueue = osMessageQueueNew(6, sizeof(MessageFromQueue),
	NULL);

	ToServerQueue = osMessageQueueNew(6, sizeof(MessageFromQueue), NULL);
	FromServerQueue = osMessageQueueNew(6, sizeof(MessageFromQueue), NULL);
	ToServerSecQueue = osMessageQueueNew(6, sizeof(MessageFromQueue),
	NULL);
	FromServerSecQueue = osMessageQueueNew(6, sizeof(MessageFromQueue),
	NULL);

	//0 - Переход от GPRS на Ethernet
	//1 - Переход от Ethernet на GPRS
	//-1 - Нет связи с внешним миром
// Временно для отладки
	osDelay(10000U);
	TCPSet mainTCP, secTCP;
	GetCopy("cmain", &mainTCP);
	mainTCP.tread = 300;
	mainTCP.twrite = 5;
	mainTCP.tque=10;
	SetCopy("cmain", &mainTCP);
	GetCopy("csec", &secTCP);
	secTCP.tread = 10;
	secTCP.twrite = 5;
	secTCP.tque=300;
	secTCP.port = 2094;
	SetCopy("csec", &secTCP);
	setTimeoutForChanel(20);
	ChangeStatus = osMessageQueueNew(6, sizeof(int), NULL);
	GetCopy("setup", &devStatus);
	if (!devStatus.Ethertnet && devStatus.Gprs) setGPRSNeed(true);

	if (devStatus.Ethertnet) {
		setGoodTCP(true);
		/* creation of FromServerTCP */
		FromServerTCPHandle = osThreadNew(StartFromServerTCP, NULL, &FromServerTCP_attributes);

		/* creation of ToServerTCP */
		ToServerTCPHandle = osThreadNew(StartToServerTCP, NULL, &ToServerTCP_attributes);
	}
	if (devStatus.Gprs) {
		setGoodGPRS(true);
		/* creation of FromServerGPRS */
		FromServerGPRSHandle = osThreadNew(StartFromServerGPRS, NULL, &FromServerGPRS_attributes);

		/* creation of ToServerGPRS */
		ToServerGPRSHandle = osThreadNew(StartToServerGPRS, NULL, &ToServerGPRS_attributes);
	}

	TechExchHandle = osThreadNew(TechExchange, NULL, &TechExch_attributes);

	if (!devStatus.Ethertnet && !devStatus.Gprs) {
		noETHandGPRS();
	}

	MessageFromQueue msg;
	for (;;) {
		osDelay(STEP_CONTROL);
		size_t lenT=osMessageQueueGetCount(ETHFromServerSecQueue);
		size_t lenF=osMessageQueueGetCount(ETHFromServerQueue);
		if (lenT!=0||lenF!=0) {
			Debug_Message(LOG_INFO, "Во второй ETH %d  записей %d", lenT,lenF);

		}
//		Debug_Message(LOG_INFO, "Цикл Transport");
		//Вначале проверяем ответы по Ethernet
		if (devStatus.Ethertnet) {
			if (osMessageQueueGet(ETHFromServerQueue, &msg, NULL, tout) == osOK) {
				if (msg.error == TRANSPORT_OK) {
					if (isGoodGPRS()) {
						//Работали по GPRS нужно его остановить и перейти на Ethernet
						stopGPRS();
					}
					osMessageQueuePut(FromServerQueue, &msg, 0, 0);
					Debug_Message(LOG_INFO, "Перенесли из ETH в основную %.20s", msg.message);

				} else {
					//Работали по Ethernet нужно запустить GPRS
					startGPRS();
					osMessageQueuePut(FromServerQueue, &msg, 0, 0);
					Debug_Message(LOG_INFO, "Перенесли из ETH в основную разрыв связи");
				}
			}
			if (osMessageQueueGet(ETHFromServerSecQueue, &msg, NULL, tout) == osOK) {
				if (msg.error == TRANSPORT_OK) {
					if (isGoodGPRS()) {
						//Работали по GPRS нужно его остановить и перейти на Ethernet
						stopGPRS();
					}
					osMessageQueuePut(FromServerSecQueue, &msg, 0, 0);
					Debug_Message(LOG_INFO, "Перенесли из ETH во вторую %.20s", msg.message);

				} else {
					//Работали по Ethernet нужно запустить GPRS
					startGPRS();
					osMessageQueuePut(FromServerSecQueue, &msg, 0, 0);
					Debug_Message(LOG_INFO, "Перенесли из ETH во вторую разрыв связи");
				}
			}
		}
		if (devStatus.Gprs) {
			//Вычитываем и пробрасываем сообщения от GPRS
			if (osMessageQueueGet(GPRSFromServerQueue, &msg, NULL, tout) == osOK) {
				if (msg.error == TRANSPORT_OK) {
					Debug_Message(LOG_INFO, "Перенесли из GPRS в основную %.20s", msg.message);
				} else {
					Debug_Message(LOG_INFO, "Перенесли из GPRS в основную разрыв связи");
				}
				osMessageQueuePut(FromServerQueue, &msg, 0, 0);
			}
			if (osMessageQueueGet(GPRSFromServerSecQueue, &msg, NULL, tout) == osOK) {
				if (msg.error == TRANSPORT_OK) {
					Debug_Message(LOG_INFO, "Перенесли из GPRS во вторую %.20s", msg.message);
				}else {
					Debug_Message(LOG_INFO, "Перенесли из GPRS во вторую разрыв связи");
				}
				osMessageQueuePut(FromServerSecQueue, &msg, 0, 0);
			}
		}
		// Пересылаем сообщения от главного модуля
		if (osMessageQueueGet(ToServerQueue, &msg, NULL, tout) == osOK) {
			if (isGoodTCP()){
				osMessageQueuePut(ETHToServerQueue, &msg, 0, 0);
				Debug_Message(LOG_INFO, "Перенесли из основной в ETH  %.20s", msg.message);
			}
			else if (isGoodGPRS()) {
				osMessageQueuePut(GPRSToServerQueue, &msg, 0, 0);
				Debug_Message(LOG_INFO, "Перенесли из основной в GPRS  %.20s", msg.message);
			} else {
				Debug_Message(LOG_INFO, "Придется удалить %.20s", msg.message);
				vPortFree(msg.message);
			}
		}
		if (osMessageQueueGet(ToServerSecQueue, &msg, NULL, tout) == osOK) {
			if (isGoodTCP()){
				osMessageQueuePut(ETHToServerSecQueue, &msg, 0, 0);
				Debug_Message(LOG_INFO, "Перенесли из второй в ETH  %.20s", msg.message);
			}
			else if (isGoodGPRS()) {
				osMessageQueuePut(GPRSToServerSecQueue, &msg, 0, 0);
				Debug_Message(LOG_INFO, "Перенесли из второй в GPRS  %.20s", msg.message);
			} else {
				Debug_Message(LOG_INFO, "Придется удалить %.20s", msg.message);
				vPortFree(msg.message);
			}
		}
	}
}

