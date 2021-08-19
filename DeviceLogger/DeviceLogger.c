/*
 * DeviceLogger.c
 *
 *  Created on: 18 авг. 2021 г.
 *      Author: rura
 */

#include "DeviceLogger.h"
#include "DeviceTime.h"
#include "DebugLogger.h"
#include <string.h>

static SubNames DevLogWork[]={
		{SHARE_SUB,"Share","\0"},
		{LOG_SUB,"Logger","\0"},
		{END_SUB,"\0","\0"}
	};
static osMutexId_t LogDevMutex;
static RingBuffer* devicelogs;
char tempBuffer[MAX_LEN_MESSAGE];
void DeviceLogInit(){
	LogDevMutex=osMutexNew(NULL);
	devicelogs=newRingBuffer(CAPACITY_MESSAGE, sizeof(DeviceLoggerMessage));
	//Если буфер переполнился то удаляем самое старое сообщение
	setOverWriteRingBuffer(devicelogs);
	if(devicelogs==NULL){
		Debug_Message(LOG_FATAL, "Невозможно создать лог устройства");
		return;
	}
	Debug_Message(LOG_INFO, "Создан лог устройства");
}
SubNames* getSubsystem(char sub){
	SubNames* result=DevLogWork;
	while (result->subsystem!=END_SUB){
		if (result->subsystem ==sub) return result;
		result++;
	}
	return NULL;
}
void DeviceLog(char subsytem,const char*message){
	if (osMutexAcquire(LogDevMutex, osWaitForever) == osOK) {
		SubNames* subName=getSubsystem(subsytem);
		if (subName!=NULL) {
			strncpy(tempBuffer,message,MAX_LEN_MESSAGE);
			tempBuffer[MAX_LEN_MESSAGE-1]=0;
			//Если такое сообщение уже посылали то и нефиг его записывать
			if (strcmp(subName->lastMessage,tempBuffer)!=0){
				strcpy(subName->lastMessage,tempBuffer);
				DeviceLoggerMessage msg;
				msg.time=GetDeviceTime();
				msg.subsystem=subsytem;
				strcpy(msg.message,tempBuffer);
				int res=RingBufferTryWrite(devicelogs, (void *)&msg);
				if (res!=RINGBUFFER_OK){
					Debug_Message(LOG_ERROR, "Невозможно записать в  лог устройства");
				}
			}
		} else {
			Debug_Message(LOG_ERROR, "Неверный номер подсистемы");
		}
		osMutexRelease(LogDevMutex);
	}
}

JSON_Object* DeviceLogToJSON(){
	JSON_Value* result=NULL;
	return result;
}

