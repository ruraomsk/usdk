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
#define SIZE_LOGGER_BUFFER 256
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
	vsnprintf(newLog->Buffer, sizeof(newLog->Buffer)-1, fmt, ap);
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
	FIL flog;
	UINT bw;
	struct sockaddr_in server;
	int socket=-1;
	server.sin_family=AF_INET;
	server.sin_port=htons(2095);
	inet_aton("192.168.115.159",&server.sin_addr.s_addr);
	int c=10;
	socket=socket(AF_INET,SOCK_STREAM,0);
	if (socket>=0){
		int err;
		do{
			err=connect(socket, (struct sockaddr* ) &server, sizeof(struct sockaddr_in));
			if (--c<0) break;
			osDelay(1000);
		} while(err!=0);
	}
	Debug_Message(LOG_INFO, "Logger запущен");
#define COUNTER 5000
	int count=COUNTER;
	size_t minimum=INT32_MAX;
	/* Infinite loop */
	for (;;) {
		if (osMessageQueueGet(DebugLoggerQueue, &msg, NULL, 10) == osOK) {
			count=COUNTER;
			minimum=msg.size<minimum?msg.size:minimum;
			snprintf(LoggerBuffer,SIZE_LOGGER_BUFFER, "%s:%6s:%6d:%6d:%s\n", TimeToString(msg.time), Debuger_Status(msg.Level),msg.size,minimum, msg.Buffer);
			while (socket>=0){
				int err=send(socket,LoggerBuffer,strlen(LoggerBuffer),0);
				if (err<0) {
					shutdown(socket, SHUT_RDWR);
					close(socket);
					socket=-1;
				}
				break;
			}
			f_open(&flog, "debug.log", FA_WRITE | FA_OPEN_ALWAYS);
			f_lseek(&flog, f_size(&flog));
			f_write(&flog, LoggerBuffer, strlen(LoggerBuffer), &bw);
			f_close(&flog);

		} else {
			count--;
			if (count <0 ){
				count=COUNTER;
				Debug_Message(LOG_INFO, "Работаем");
			}
		}
	}
}

