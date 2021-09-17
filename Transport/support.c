/*
 * ReadWrite.c
 *
 *  Created on: 27 авг. 2021 г.
 *      Author: rura
 */

#include <sockets.h>
#include <stdlib.h>

#include "Transport.h"
#include "Files.h"
#include "core_json.h"
#include "CommonData.h"
extern int TCPError;
extern int GPRSError;
extern int GPRSNeed;			//Готовность работы GPRS если есть
extern int FromServerTCPStart;
extern int FromServerGPRSStart;
extern osMutexId_t TransportMutex;

char* makeConnectString(const size_t buffersize,char *typestring){
	char* result=NULL;
	if (osMutexAcquire(TransportMutex, osWaitForever)==osOK){
		DeviceStatus devStatus;
		js_write w;
		GetCopy("setup", &devStatus);
		js_write_start(&w, buffersize);
		js_write_int(&w, "id", devStatus.ID);
		js_write_string(&w, "type", typestring);
		result=w.start;
		osMutexRelease(TransportMutex);
	}
	return result;
}
void setFromServerTCPStart(int v){
	if (osMutexAcquire(TransportMutex, osWaitForever)==osOK){
		FromServerTCPStart=v;
		osMutexRelease(TransportMutex);
	}
}
void setFromServerGPRSStart(int v){
	if (osMutexAcquire(TransportMutex, osWaitForever)==osOK){
		FromServerGPRSStart=v;
		osMutexRelease(TransportMutex);
	}
}
void setGPRSNeed(int v){
	if (osMutexAcquire(TransportMutex, osWaitForever)==osOK){
		GPRSNeed=v;
		osMutexRelease(TransportMutex);
	}
}
void setGoodTCP(int v){
	if (osMutexAcquire(TransportMutex, osWaitForever)==osOK){
		TCPError=v;
		osMutexRelease(TransportMutex);
	}
}
void setGoodGPRS(int v){
	if (osMutexAcquire(TransportMutex, osWaitForever)==osOK){
		GPRSError=v;
		osMutexRelease(TransportMutex);
	}
}
int isGoodTCP(){
	return TCPError;
}
int isGoodGPRS(){
	return GPRSError;
}
void BadTCP(char *buffer,int socket,osMessageQueueId_t que) {
	setGoodTCP(0);
	vPortFree(buffer);
	MessageFromQueue msg;
	msg.error = TRANSPORT_ERROR;
	msg.message = NULL;
	osMessageQueuePut(que, &msg, 0, 0);
	if(socket<0) return;
	shutdown(socket, SHUT_RDWR);
	close(socket);
}
void BadGPRS(char *buffer,int socket,osMessageQueueId_t que) {
	setGoodGPRS(0);
	vPortFree(buffer);
	MessageFromQueue msg;
	msg.error = TRANSPORT_ERROR;
	msg.message = NULL;
	osMessageQueuePut(que, &msg, 0, 0);
	if(socket<0) return;
	shutdown(socket, SHUT_RDWR);
	close(socket);
}
