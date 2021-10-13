/*
 * ReadWrite.c
 *
 *  Created on: 27 авг. 2021 г.
 *      Author: rura
 */

#include <sockets.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "Transport.h"
#include "Files.h"
#include "core_json.h"
#include "CommonData.h"
extern bool TCPError;
extern bool GPRSError;
extern bool GPRSNeed;			//Готовность работы GPRS если есть
extern bool ToServerTCPStart;
extern bool ToServerGPRSStart;

void deleteEnter(char *buffer) {
	char *ptr = strchr(buffer, '\n');
	if (ptr != NULL) *ptr = 0;
}
char* makeConnectString(char *typestring) {
	char *result = NULL;
	DeviceStatus devStatus;
	GetCopy(DeviceStatusName, &devStatus);
	result = pvPortMalloc(LenConnectString);
	if (result == NULL) return result;
	snprintf(result, LenConnectString, "connect,%d,%s", devStatus.ID, typestring);
	return result;
}
void setToServerTCPStart(bool v) {
	ToServerTCPStart = v;
}
void setToServerGPRSStart(bool v) {
	ToServerGPRSStart = v;
}
void setGPRSNeed(bool v) {
	GPRSNeed = v;
}
void setGoodTCP(bool v) {
	TCPError = v;
}
void setGoodGPRS(bool v) {
	GPRSError = v;
}
bool isGoodTCP() {
	return TCPError;
}
bool isGoodGPRS() {
	return GPRSError;
}
void BadTCP(int socket) {
	setGoodTCP(false);
	setToServerTCPStart(false);
	if (socket < 0) return;
	shutdown(socket, SHUT_RDWR);
	close(socket);
}
void BadGPRS(char *buffer, int socket, osMessageQueueId_t que) {
	setGoodGPRS(false);
	setToServerGPRSStart(false);
	vPortFree(buffer);
	MessageFromQueue msg = { .message = NULL };
	msg.error = TRANSPORT_ERROR;
	msg.message = NULL;
	osMessageQueuePut(que, &msg, 0, 0);
	if (socket < 0) return;
	shutdown(socket, SHUT_RDWR);
	close(socket);
}
