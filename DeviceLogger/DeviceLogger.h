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
#include "core_json.h"


#define MAX_LEN_MESSAGE 80    		//Максимальная длина строки сообщения
#define CAPACITY_MESSAGES 100		//Мксимальное  число элементов в буфере логов

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
	time_t time;
	char subsystem;
	char message[MAX_LEN_MESSAGE];
} DeviceLoggerMessage;

void DeviceLog(char subsytem,char*fmt,...);
void DeviceLogInit(void);
bool LogLineToJsonSubString(js_write *w);

#endif /* DEVICELOGGER_H_ */
