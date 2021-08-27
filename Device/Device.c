/*
 * Device.c
 *
 *  Created on: 27 авг. 2021 г.
 *      Author: rura
 */
#include "DeviceLogger.h"

DeviceStatus deviceStatus;
static osMessageQueueId_t ToServerQueue;
static osMessageQueueId_t FromServerQueue;
static osMutexId_t DeviceMutex;
char *DeviceBuffer;
static int transport=0;

void mainDeviceLoop(void){
	ToServerQueue = osMessageQueueNew(16, sizeof(MessageFromQueue), NULL);
	FromServerQueue = osMessageQueueNew(16, sizeof(MessageFromQueue), NULL);
	DeviceMutex = osMutexNew(NULL);
	LoggerBuffer=malloc(DeviceBufferSize);

	deviceStatus=readSetup( "setupdevice");
	for(;;){
		MessageFromQueue msg=makeConnectString(DeviceBuffer,"ETH", deviceStatus)
		if (deviceStatus.Ethertnet) {
			//В начале пробуем соедениться по Ethernet



		}


		osMessageQueuePut(ToServerQueue, newLog, 0, 0);

	}

}
