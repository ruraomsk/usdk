/*
 * Logger.c
 *
 *  Created on: Aug 10, 2021
 *      Author: rura
 */

#include "DebugLogger.h"
#include <lwip.h>
#include "main.h"
#include <string.h>
#include "cmsis_os.h"
#include "ff.h"
#include "Files.h"
#include <stdarg.h>
#include <stdio.h>
#include <sockets.h>
#define SIZE_LOGGER_BUFFER 200
osMessageQueueId_t DebugLoggerQueue;
osThreadAttr_t Debug_attributes = {  .stack_size = 2048 * 4, .priority =(osPriority_t) osPriorityRealtime,.name="deblog" };

char LoggerBuffer [ SIZE_LOGGER_BUFFER ];
#define LIMIT_DEBUG_LOGGER_SIZE_Kb  5
void Debug_Init() {
	DebugLoggerQueue = osMessageQueueNew(128, sizeof(DebugLoggerMsg), NULL);
	osThreadNew(DebugLoggerLoop, 0, &Debug_attributes);

}
HeapStats_t pxHeapStats;
void Debug_Message(int level, char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	DebugLoggerMsg newLog;
	vPortGetHeapStats(&pxHeapStats);
	vsnprintf(newLog.Buffer, sizeof(newLog.Buffer) - 1, fmt, ap);
	newLog.Level = level;
	newLog.time = GetDeviceTime();
	newLog.size = pxHeapStats.xAvailableHeapSpaceInBytes;
	if (osMessageQueueGetSpace(DebugLoggerQueue) > 1) {
		osMessageQueuePut(DebugLoggerQueue, &newLog, 0, osWaitForever);
	}
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

DebugLoggerMsg dlmsg;
void DebugLoggerLoop(void *arg) {

//	FIL flog;
//	UINT bw;
	struct sockaddr_in server;
	int sock = -1;
	server.sin_family = AF_INET;
	server.sin_port = htons(2095);
	inet_aton("192.168.115.159", &server.sin_addr.s_addr);
	int c = 100;
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock >= 0) {
		int err;
		do {
			err = connect(sock, (struct sockaddr* ) &server, sizeof(struct sockaddr_in));
			if (--c < 0) break;
			osDelay(1000);
		} while (err != 0);
	}
	Debug_Message(LOG_INFO, "Logger запущен");
#define COUNTER 3000
	int count = COUNTER;
	size_t minimum = INT32_MAX;
	/* Infinite loop */
	count = COUNTER;
	for (;;) {
		if (osMessageQueueGetCount(DebugLoggerQueue) != 0) {
			osMessageQueueGet(DebugLoggerQueue, &dlmsg, NULL, osWaitForever);
			minimum = dlmsg.size < minimum ? dlmsg.size : minimum;
			snprintf(LoggerBuffer, SIZE_LOGGER_BUFFER, "%s:%d:%.1s:%s\n", ShortTimeToString(dlmsg.time),minimum,
					Debuger_Status(dlmsg.Level), dlmsg.Buffer);
			while (sock >= 0) {

				send(sock, LoggerBuffer, strlen(LoggerBuffer), 0);
				break;
			}
//			//			f_open(&flog, "debug.log", FA_WRITE | FA_OPEN_ALWAYS);
//			//			if (f_size(&flog)>LIMIT_DEBUG_LOGGER_SIZE_Kb*1024){
//			//				f_close(&flog);
//			//				f_unlink("debug.log");
//			//				f_open(&flog, "debug.log", FA_WRITE | FA_OPEN_ALWAYS);
//			//			}
//			//			f_lseek(&flog, f_size(&flog));
//			//			f_write(&flog, LoggerBuffer, strlen(LoggerBuffer), &bw);
//			//			f_close(&flog);
//
		} else {
			count--;
			osDelay(100U);
			if (count < 0) {
				count = COUNTER;
				Debug_Message(LOG_INFO, "Работаем");
			}

		}
	}
}

