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
#include "lwip.h"
#include "parson.h"

extern osMessageQueueId_t DebugLoggerQueue;
extern int ReadyLogger;

char LoggerBuffer[512];
struct {
	ip_addr_t ipAddr;
	int adr1, adr2, adr3, adr4;
	unsigned int port;
} debugSetup;

void Debug_Message(int level, char *message) {
	DebugLoggerMsg *newLog;
	newLog = malloc(sizeof(DebugLoggerMsg));
	memset(newLog->Buffer, 0, sizeof(newLog->Buffer));
	int len = strlen(message);
	if (len > (sizeof(newLog->Buffer) - 1)) {
		len = sizeof(newLog->Buffer) - 1;
	}
	memcpy(newLog->Buffer, message, len);
	newLog->Level = level;
	newLog->time = GetDeviceTime();
	osMessageQueuePut(DebugLoggerQueue, newLog, 0, 0);
	free(newLog);
}
char* Debuger_Status(int level) {
	switch (level) {
	case LOG_ERROR:
		return "ERROR";
	case LOG_FATAL:
		return "FATAL";
	case LOG_INFO:
		return "INFO ";
	default:
		return "UNDEF";
	}
}
void DebugLoggerLoop() {
	struct udp_pcb *udp;
	DebugReadSetup();
	DebugWriteSetup();
	udp = udp_new();
	udp_connect(udp, &debugSetup.ipAddr, debugSetup.port);
	ReadyLogger = 1;
	Debug_Message(LOG_INFO, "Logger запущен");
	/* Infinite loop */
	for (;;) {
		DebugLoggerMsg msg;
		if (osMessageQueueGet(DebugLoggerQueue, &msg, NULL, 0) == osOK) {
			sprintf(LoggerBuffer, "%s:%6s:%s\n\r", TimeToString(msg.time),
					Debuger_Status(msg.Level), msg.Buffer);
			struct pbuf *udp_buffer = pbuf_alloc(PBUF_TRANSPORT,
					strlen(LoggerBuffer), PBUF_RAM);
			if (udp_buffer != NULL) {
				memcpy(udp_buffer->payload, LoggerBuffer, strlen(LoggerBuffer));
				udp_send(udp, udp_buffer);
				pbuf_free(udp_buffer);
			}
		}
		osDelay(100);
	}
}
void DebugReadSetup(void) {
	JSON_Value *root = ShareGetJson("debug");
	JSON_Object *object = json_value_get_object(root);
	debugSetup.port = (int) json_object_get_number(object, "port");
	sscanf(json_object_get_string(object, "ip"), "%d.%d.%d.%d",
			&debugSetup.adr1, &debugSetup.adr2, &debugSetup.adr3,
			&debugSetup.adr4);
	IP_ADDR4(&debugSetup.ipAddr, debugSetup.adr1, debugSetup.adr2,
			debugSetup.adr3, debugSetup.adr4);
}

void DebugWriteSetup(void) {
	JSON_Value *root_value = json_value_init_object();
	JSON_Object *root_object = json_value_get_object(root_value);
	char *serialized_string = NULL;
	sprintf(LoggerBuffer, "%d.%d.%d.%d", debugSetup.adr1, debugSetup.adr2,
			debugSetup.adr3, debugSetup.adr4);
	json_object_set_string(root_object, "ip", LoggerBuffer);
	json_object_set_number(root_object, "port", debugSetup.port);
	ShareSetJson("debug", root_value);
}

