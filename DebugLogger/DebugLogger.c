/*
 * Logger.c
 *
 *  Created on: Aug 10, 2021
 *      Author: rura
 */

#include "DebugLogger.h"

#include "main.h"
#include <string.h>
#include "cmsis_os.h"
#include "ff.h"
#include "Files.h"
#include <stdarg.h>
#include <stdio.h>
#define SIZE_LOGGER_BUFFER 256
extern int ReadyFiles;
osMessageQueueId_t DebugLoggerQueue;
char *LoggerBuffer = NULL;
void Debug_Init() {
	DebugLoggerQueue = osMessageQueueNew(32, sizeof(DebugLoggerMsg), NULL);
	LoggerBuffer = pvPortMalloc(SIZE_LOGGER_BUFFER);
}
HeapStats_t pxHeapStats;
void Debug_Message(int level, char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	DebugLoggerMsg *newLog;
	vPortGetHeapStats(&pxHeapStats);
	newLog = pvPortMalloc(sizeof(DebugLoggerMsg));
	if (newLog==NULL) return;
	vsnprintf(newLog->Buffer, sizeof(newLog->Buffer), fmt, ap);
	newLog->Level = level;
	newLog->time = GetDeviceTime();
	newLog->size=pxHeapStats.xAvailableHeapSpaceInBytes;
	osMessageQueuePut(DebugLoggerQueue, newLog, 0, 0);
	vPortFree(newLog);
}
char* Debuger_Status(int level) {
	switch (level) {
	case LOG_ERROR:
		return "ERROR";
	case LOG_FATAL:
		return "FATAL";
	case LOG_DEBUG:
		return "DEBUG";
	case LOG_INFO:
		return "INFO ";
	default:
		return "UNDEF";
	}
}
DebugLoggerMsg msg;
void DebugLoggerLoop() {
	while (!ReadyFiles) {
		osDelay(1000);
	}
	FIL flog;
	UINT bw;
	f_open(&flog, "debug.log", FA_CREATE_ALWAYS | FA_WRITE);
	Debug_Message(LOG_INFO, "Logger запущен");
	/* Infinite loop */
	for (;;) {
		if (osMessageQueueGet(DebugLoggerQueue, &msg, NULL, 10) == osOK) {
			snprintf(LoggerBuffer,SIZE_LOGGER_BUFFER, "%s:%6s:%6d:%s\n", TimeToString(msg.time), Debuger_Status(msg.Level),msg.size, msg.Buffer);
			f_write(&flog, LoggerBuffer, strlen(LoggerBuffer), &bw);
			f_sync(&flog);
		} else {
			osDelay(100);
		}
	}
}

