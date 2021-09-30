/*
 * ReadWrite.c
 *
 *  Created on: 27 авг. 2021 г.
 *      Author: rura
 */

#include <sockets.h>
#include <stdlib.h>
#include <string.h>
#include "Transport.h"
#include "Files.h"
#include "core_json.h"
#include "CommonData.h"
extern int TCPError;
extern int GPRSError;
extern int GPRSNeed;			//Готовность работы GPRS если есть
extern int ToServerTCPStart;
extern int ToServerGPRSStart;
extern osMutexId_t TransportMutex;

void deleteEnter(void* buffer){
	char*ptr=strchr(buffer,'\n');
	if(ptr!=NULL) *ptr=0;
}
char* makeConnectString(const size_t buffersize, char *typestring) {
	char *result = NULL;
		DeviceStatus devStatus;
		js_write w;
		GetCopy("setup", &devStatus);
		js_write_start(&w, buffersize);
		js_write_value_start(&w, "connect");
		js_write_int(&w, "id", devStatus.ID);
		js_write_string(&w, "type", typestring);
		js_write_value_end(&w);
		js_write_end(&w);
		result = w.start;
	return result;
}
void setToServerTCPStart(bool v) {
	if (osMutexAcquire(TransportMutex, osWaitForever) == osOK) {
		ToServerTCPStart = v;
		osMutexRelease(TransportMutex);
	}
}
void setToServerGPRSStart(bool v) {
	if (osMutexAcquire(TransportMutex, osWaitForever) == osOK) {
		ToServerGPRSStart = v;
		osMutexRelease(TransportMutex);
	}
}
void setGPRSNeed(bool v) {
	if (osMutexAcquire(TransportMutex, osWaitForever) == osOK) {
		GPRSNeed = v;
		osMutexRelease(TransportMutex);
	}
}
void setGoodTCP(bool v) {
	if (osMutexAcquire(TransportMutex, osWaitForever) == osOK) {
		TCPError = v;
		osMutexRelease(TransportMutex);
	}
}
void setGoodGPRS(bool v) {
	if (osMutexAcquire(TransportMutex, osWaitForever) == osOK) {
		GPRSError = v;
		osMutexRelease(TransportMutex);
	}
}
bool isGoodTCP() {
	return TCPError;
}
bool isGoodGPRS() {
	return GPRSError;
}
void BadTCP(char *buffer, int socket, osMessageQueueId_t que) {
	setGoodTCP(false);
	setToServerTCPStart(false);
	vPortFree(buffer);
	MessageFromQueue msg={.message=NULL};
	msg.error = TRANSPORT_ERROR;
	msg.message = NULL;
	osMessageQueuePut(que, &msg, 0, 0);
	if (socket < 0) return;
	shutdown(socket, SHUT_RDWR);
	close(socket);
}
void BadGPRS(char *buffer, int socket, osMessageQueueId_t que) {
	setGoodGPRS(false);
	setToServerGPRSStart(false);
	vPortFree(buffer);
	MessageFromQueue msg={.message=NULL};
	msg.error = TRANSPORT_ERROR;
	msg.message = NULL;
	osMessageQueuePut(que, &msg, 0, 0);
	if (socket < 0) return;
	shutdown(socket, SHUT_RDWR);
	close(socket);
}
