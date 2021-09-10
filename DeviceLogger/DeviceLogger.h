/*
 * DeviceLogger.h
 *
 *  Created on: 18 авг. 2021 г.
 *      Author: rura
 */

#ifndef DEVICELOGGER_H_
#define DEVICELOGGER_H_

#include "DeviceTime.h"
#include "RingBuffer.h"
#include "parson.h"

#define MAX_LEN_MESSAGE 120    		//Максимальная длина строки сообщения
#define CAPACITY_MESSAGES 100		//Мксимальное  число элементов в буфере логов
#define NEED_FOR_SAVING  10000

enum {
	SUB_END =0,
	SUB_TRANSPORT,
	SUB_FILES,
};

typedef struct {
	char subsystem;
	char name[20];
	char *lastMessage;
} SubNames;

typedef struct {
	dev_time time;
	char subsystem;
	char message[MAX_LEN_MESSAGE];
} DeviceLoggerMessage;

void DeviceLog(char subsytem,char*fmt,...);
void DeviceLogInit(void);
JSON_Value* DeviceLogToJSON(void);


#endif /* DEVICELOGGER_H_ */
