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
//DeviceStatus deviceStatus;

int GPRSNeed = 0;			//Готовность работы GPRS если есть
int FromServerTCPStart = 0;
int FromServerGPRSStart = 0;
int TCPError = 0;
int GPRSError = 0;

//Очереди для Ethernet
osMessageQueueId_t ToServerQueue;
osMessageQueueId_t FromServerQueue;
osMessageQueueId_t ToServerSecQueue;
osMessageQueueId_t FromServerSecQueue;

//Очереди для GPRS
osMessageQueueId_t GPRSToServerQueue;
osMessageQueueId_t GPRSFromServerQueue;
osMessageQueueId_t GPRSToServerSecQueue;
osMessageQueueId_t GPRSFromServerSecQueue;

//Очереди от основной программы управления
osMessageQueueId_t MainChangeStatus;
osMessageQueueId_t MainToServerQueue;
osMessageQueueId_t MainFromServerQueue;
osMessageQueueId_t MainToServerSecQueue;
osMessageQueueId_t MainFromServerSecQueue;

osMutexId_t TransportMutex;
/* Definitions for ToServerTCP */
osThreadId_t ToServerTCPHandle;
const osThreadAttr_t ToServerTCP_attributes = { .name = "ToTCP",
		.stack_size = 256 * 4, .priority = (osPriority_t) osPriorityNormal, };
/* Definitions for FromServerTCP */
osThreadId_t FromServerTCPHandle;
const osThreadAttr_t FromServerTCP_attributes = { .name = "FromTCP",
		.stack_size = 256 * 4, .priority = (osPriority_t) osPriorityNormal, };
/* Definitions for ToServerGPRS */
osThreadId_t ToServerGPRSHandle;
const osThreadAttr_t ToServerGPRS_attributes = { .name = "ToGPRS",
		.stack_size = 256*4 , .priority = (osPriority_t) osPriorityNormal, };
/* Definitions for FromServerGPRS */
osThreadId_t FromServerGPRSHandle;
const osThreadAttr_t FromServerGPRS_attributes = { .name = "FromGPRS",
		.stack_size = 256 * 4, .priority = (osPriority_t) osPriorityNormal, };


#define tout 10
void startGPRS() {
	MessageFromQueue msg;
	int c = 1;
	msg.error = TRANSPORT_STOP;
	osMessageQueuePut(ToServerQueue, &msg, 0, 0);
	osMessageQueuePut(ToServerSecQueue, &msg, 0, 0);
	osMessageQueuePut(MainChangeStatus, &c, 0, 0);
	DeviceLog(SUB_TRANSPORT, "Переключились с ETH на GPRS");
	setGPRSNeed(1);
}
void stopGPRS() {
	MessageFromQueue msg;
	int c = 0;
	msg.error = TRANSPORT_STOP;
	osMessageQueuePut(GPRSToServerQueue, &msg, 0, 0);
	osMessageQueuePut(GPRSToServerSecQueue, &msg, 0, 0);
	osMessageQueuePut(MainChangeStatus, &c, 0, 0);
	DeviceLog(SUB_TRANSPORT, "Переключились с GPRS на ETH");
	setGPRSNeed(0);

}
void noETHandGPRS() {
	MessageFromQueue msg;
	int c = -1;
	osMessageQueuePut(MainChangeStatus, &c, 0, 0);
	DeviceLog(SUB_TRANSPORT, "Нет устройств связи");
	for (;;) {
		if (osMessageQueueGet(MainToServerQueue, &msg, NULL, tout) == osOK) {
			DeviceLog(SUB_TRANSPORT, "Пришло сообщение на главный ");
			MessageFromQueue ms;
			ms.error = TRANSPORT_STOP;
			osMessageQueuePut(MainFromServerQueue, &ms, 0, 0);
		}
		if (osMessageQueueGet(MainToServerSecQueue, &msg, NULL, tout) == osOK) {
			DeviceLog(SUB_TRANSPORT, "Пришло сообщение на второй ");
			MessageFromQueue ms;
			ms.error = TRANSPORT_STOP;
			osMessageQueuePut(MainFromServerSecQueue, &ms, 0, 0);
		}
	}
}
void StartToServerTCP(void *argument) {
	while (1) {
		while (FromServerTCPStart == 0) {
			osDelay(10);
		}
//		Debug_Message(LOG_INFO, "Запускаем ToServerTCP");
		ToServerTCPLoop();
//		Debug_Message(LOG_INFO, "остановился ToServerTCP");
		osDelay(STEP_TCP);

	}
}
void StartFromServerTCP(void *argument) {
	while (1) {
//		Debug_Message(LOG_INFO, "Запускаем FromServerTCP");
		FromServerTCPLoop();
		setFromServerTCPStart(0);
//		Debug_Message(LOG_INFO, "остановился FromServerTCP");
		osDelay(STEP_TCP);

	}
}
void StartToServerGPRS(void *argument) {
	while (1) {
		while (GPRSNeed == 0 || FromServerGPRSStart == 0) {
			osDelay(10);
		}
//		Debug_Message(LOG_INFO, "Запускаем ToServerGPRS");
		ToServerGPRSLoop();
//		Debug_Message(LOG_INFO, "остановился ToServerGPRS");
		osDelay(STEP_GPRS);

	}
}

void StartFromServerGPRS(void *argument) {
	while (1) {
		while (GPRSNeed == 0) {
//			Debug_Message(LOG_INFO, "Wait FromServerGPRS");

			osDelay(100);
		}
//		Debug_Message(LOG_INFO, "Запускаем FromServerGPRS");
		FromServerGPRSLoop();
//		Debug_Message(LOG_INFO, "остановился FromServerGPRS");
		setFromServerGPRSStart(0);
		osDelay(STEP_GPRS);

	}
}

void mainTransportLoop(void) {
	TransportMutex = osMutexNew(NULL);
	ToServerQueue = osMessageQueueNew(6, sizeof(MessageFromQueue), NULL);
	FromServerQueue = osMessageQueueNew(6, sizeof(MessageFromQueue), NULL);
	ToServerSecQueue = osMessageQueueNew(6, sizeof(MessageFromQueue), NULL);
	FromServerSecQueue = osMessageQueueNew(6, sizeof(MessageFromQueue), NULL);

	GPRSToServerQueue = osMessageQueueNew(6, sizeof(MessageFromQueue), NULL);
	GPRSFromServerQueue = osMessageQueueNew(6, sizeof(MessageFromQueue), NULL);
	GPRSToServerSecQueue = osMessageQueueNew(6, sizeof(MessageFromQueue),
			NULL);
	GPRSFromServerSecQueue = osMessageQueueNew(6, sizeof(MessageFromQueue),
			NULL);

	MainToServerQueue = osMessageQueueNew(6, sizeof(MessageFromQueue), NULL);
	MainFromServerQueue = osMessageQueueNew(6, sizeof(MessageFromQueue), NULL);
	MainToServerSecQueue = osMessageQueueNew(6, sizeof(MessageFromQueue),
			NULL);
	MainFromServerSecQueue = osMessageQueueNew(6, sizeof(MessageFromQueue),
			NULL);

	//0 - Переход от GPRS на Ethernet
	//1 - Переход от Ethernet на GPRS
	//-1 - Нет связи с внешним миром

	MainChangeStatus = osMessageQueueNew(6, sizeof(int), NULL);

	/* creation of FromServerGPRS */
	FromServerGPRSHandle = osThreadNew(StartFromServerGPRS, NULL,
			&FromServerGPRS_attributes);
	/* creation of FromServerTCP */
	FromServerTCPHandle = osThreadNew(StartFromServerTCP, NULL,
			&FromServerTCP_attributes);

	/* creation of ToServerTCP */
	ToServerTCPHandle = osThreadNew(StartToServerTCP, NULL,
			&ToServerTCP_attributes);


	/* creation of ToServerGPRS */
	ToServerGPRSHandle = osThreadNew(StartToServerGPRS, NULL,
			&ToServerGPRS_attributes);


	DeviceStatus deviceStatus = readSetup("setup");
	if(deviceStatus.ID<0) {
		Debug_Message(LOG_FATAL, "Transport нет настроек устройства");
		return;
	}
	if (!deviceStatus.Ethertnet && !deviceStatus.Gprs) {
		noETHandGPRS();
	}

	MessageFromQueue msg;
	for (;;) {
		osDelay(250);
		//Вначале проверяем ответы по Ethernet
		if (osMessageQueueGet(FromServerQueue, &msg, NULL, tout) == osOK) {
			if (msg.error == TRANSPORT_OK) {
				if (GPRSNeed == 1) {
					//Работали по GPRS нужно его остановить и перейти на Ethernet
					stopGPRS();
				}
				osMessageQueuePut(MainFromServerQueue, &msg, 0, 0);
			} else {
				if (GPRSNeed == 0) {
					//Работали по Ethernet нужно запустить GPRS
					startGPRS();
				}
			}
		}
		if (osMessageQueueGet(FromServerSecQueue, &msg, NULL, tout) == osOK) {
			if (msg.error == TRANSPORT_OK) {
				if (GPRSNeed == 1) {
					//Работали по GPRS нужно его остановить и перейти на Ethernet
					stopGPRS();
				}
				osMessageQueuePut(MainFromServerSecQueue, &msg, 0, 0);
			} else {
				if (GPRSNeed == 0) {
					//Работали по Ethernet нужно запустить GPRS
					startGPRS();
				}
			}
		}
		if (GPRSNeed == 1) {
			//Вычитываем и пробрасываем сообщения от GPRS
			if (osMessageQueueGet(GPRSFromServerQueue, &msg, NULL, tout)
					== osOK) {
				osMessageQueuePut(MainFromServerQueue, &msg, 0, 0);
			}
			if (osMessageQueueGet(GPRSFromServerSecQueue, &msg, NULL, tout)
					== osOK) {
				osMessageQueuePut(MainFromServerSecQueue, &msg, 0, 0);
			}
		} else {
			//Вычитываем и отбрасываем сообщения от GPRS
			osMessageQueueGet(GPRSFromServerQueue, &msg, NULL, tout);
			osMessageQueueGet(GPRSFromServerSecQueue, &msg, NULL, tout);
		}
		// Пересылаем сообщения от главного модуля
		if (osMessageQueueGet(MainToServerQueue, &msg, NULL, tout) == osOK) {
			if (GPRSNeed == 0)
				osMessageQueuePut(ToServerQueue, &msg, 0, 0);
			else
				osMessageQueuePut(GPRSToServerQueue, &msg, 0, 0);
		}
		if (osMessageQueueGet(MainToServerSecQueue, &msg, NULL, tout) == osOK) {
			if (GPRSNeed == 0)
				osMessageQueuePut(ToServerSecQueue, &msg, 0, 0);
			else
				osMessageQueuePut(GPRSToServerSecQueue, &msg, 0, 0);
		}
	}
}

