/*
 * DeviceLogger.h
 *
 *  Created on: 18 авг. 2021 г.
 *      Author: rura
 */

#ifndef DEVICELOGGER_H_
#define DEVICELOGGER_H_

#include "RingBuffer.h"
#include "parson.h"

#define MAX_LEN_MESSAGE 60    		//Максимальная длина строки сообщения
#define CAPACITY_MESSAGE 250		//Мксимальное  число элементов в буфере логов
#define END_SUB 	0
#define SHARE_SUB 	1
#define LOG_SUB 	2
typedef struct {
	char subsystem;
	char name[20];
	char lastMessage[MAX_LEN_MESSAGE];
} SubNames;
typedef struct {
	unsigned long int time;
	char subsystem;
	char message[MAX_LEN_MESSAGE];
} DeviceLoggerMessage;

void DeviceLog(char subsytem,const char*message);
void DeviceLogInit(void);
JSON_Object* DeviceLogToJSON(void);


#endif /* DEVICELOGGER_H_ */
