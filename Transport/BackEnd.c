/*
 * BackEnd.c
 *
 *  Created on: 19 окт. 2021 г.
 *      Author: rura
 */
#include "DebugLogger.h"
#include "DeviceLogger.h"
#include "DeviceTime.h"
#include "Transport.h"
#include "sockets.h"
#include "Files.h"
#include "CommonData.h"
#include "Common_private.h"
#include "getset.h"

#define SignalKeepAlive 1
#define SignalError		2
#define SignalGPS		3
#define SignalStatus	4
#define SignalSfdk		5
typedef struct {
	char *name;
	CallBackParam cp;
	uint32_t tiks;
	osTimerId_t timer;
} BackPoint;

// @formatter:off
BackPoint bp[]={
		{.name="error",.cp={.Signal=SignalError},.tiks=1000U},
		{.name="gps",.cp={.Signal=SignalGPS},.tiks=1000U},
		{.name="status",.cp={.Signal=SignalStatus},.tiks=1000U},
		{.name="sfdk",.cp={.Signal=SignalSfdk},.tiks=1000U},
		{.name=NULL},
};
// @formatter:on
char *BackEndBuffer;
char *BackEndName;
extern osMessageQueueId_t toServerMessageHandle;
osTimerId_t keepAlive;

uint16_t BackEndMessage;

CallBackParam cpKeepAlive;
CallBackParam cpOneSec;

ErrorSet backError;
GPSSet backGPS;
StatusSet backStatus;
NowState backState;
bool sfdk = false;

void BackEndStop() {
	osTimerStop(keepAlive);
	for (int i = 0; bp[i].name!=NULL; ++i) {
		osTimerStop(bp[i].timer);
	}
	osMessageQueueReset(toServerMessageHandle);
}
void messageError() {
	ErrorSet es;
	GetCopy(ErrorSetName, &es);
	if (memcmp(&backError, &es, sizeof(ErrorSet)) != 0) {
		give_me_Error(BackEndBuffer);
		GetCopy(ErrorSetName, &backError);
	}
}
void messageGPS() {
	GPSSet gs;
	GetCopy(GPSSetName, &gs);
	if (memcmp(&backGPS, &gs, sizeof(GPSSet)) != 0) {
		give_me_GPS(BackEndBuffer);
		GetCopy(GPSSetName, &backGPS);
	}
}
void messageStatus() {
	StatusSet ss;
	GetCopy(StatusSetName, &ss);
	if (memcmp(&backStatus, &ss, sizeof(StatusSet)) != 0) {
		give_me_Status(BackEndBuffer);
		GetCopy(StatusSetName, &backStatus);
	}
}
void messageSFDK() {
	NowState ns;
	GetCopy(NowStateName, &ns);
	if(ns.comdu.IsReqSFDK1){
		if (!sfdk) {
			sfdk=true;
			GetCopy(NowStateName,&backState);
		}
	}
	if(!ns.comdu.IsReqSFDK1){
		if (sfdk) {
			sfdk=false;
		}
	}
	if(!sfdk) return;
	if (memcmp(&backState, &ns, sizeof(NowState)) != 0) {
		MessageStateDevice(BackEndBuffer);
		GetCopy(NowStateName,&backState);
	}
}

void BackEndInit(char *buffer, char *name) {
	BackEndBuffer = buffer;
	BackEndName = name;
	clearErrorSet(&backError);
	clearGPSSet(&backGPS);
	clearStatusSet(&backStatus);
	clearNowState(&backState);
	TCPSet tcpSet;
	GetCopy(TCPSetSecName, &tcpSet);
	cpKeepAlive.Signal = SignalKeepAlive;
	cpKeepAlive.QueueId = toServerMessageHandle;
	keepAlive = osTimerNew(CallbackQueue, osTimerOnce, &cpKeepAlive, NULL);
	osTimerStart(keepAlive, tcpSet.tque * 1000U);
	for (int i = 0; bp[i].name!=NULL; ++i) {
		bp[i].cp.QueueId=toServerMessageHandle;
		bp[i].timer=osTimerNew(CallbackQueue, osTimerPeriodic, &bp[i].cp, NULL);
		osTimerStart(bp[i].timer,bp[i].tiks);
	}

}
bool BackEndStepOne() {
	osStatus_t res = osMessageQueueGet(toServerMessageHandle, &BackEndMessage, NULL, osWaitForever);
	if (res != osOK) {
		Debug_Message(LOG_ERROR, "%s нет сообщения или сброс ", BackEndName);
		return false;
	}
	*BackEndBuffer = '\0';
	switch (BackEndMessage) {
	case SignalKeepAlive:
		MessageStateDevice(BackEndBuffer);
		break;
	case SignalError:
		messageError();
		break;
	case SignalGPS:
		messageGPS();
		break;
	case SignalStatus:
		messageStatus();
		break;
	case SignalSfdk:
		messageSFDK();
		break;
	default:
		break;
	}
	return true;
}
bool BackEndStepTwo() {
	TCPSet tcpSet;
	GetCopy(TCPSetSecName, &tcpSet);
	switch (BackEndMessage) {
	case SignalKeepAlive:
		osTimerStart(keepAlive, tcpSet.tque * 1000U);
		break;
	default:
		break;
	}
	return true;
}
