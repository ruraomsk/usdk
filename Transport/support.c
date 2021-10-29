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

bool MakeReplay(bool *conn,char* buffer,char* name){
		if (!*conn && isConnect(buffer)) {
			prepareConnectMessage(buffer);
			MessageConfirm(buffer);
			*conn = true;
			return true;
		}
		if (!*conn && !isConnect(buffer)) {
			Debug_Message(LOG_ERROR, "%s ошибка подключения", name);
			return false;
		}
		if (isGive_Me_State(buffer)) {
			prepareGiveMeState(buffer);
			MessageStateDevice(buffer);
			return true;
		}
		if(isControlCommand(buffer)){
			if(!doControlCommand(buffer)){
				Debug_Message(LOG_ERROR, "%s Неизвестная команда управления %s", name,buffer);
				strcpy(buffer,"unknown set-you");
			}
			return true;
		}
		if(isGiveMeCommand(buffer)){
			if (doGiveCommand(buffer) == NULL) {
				Debug_Message(LOG_ERROR, "%s Неизвестная команда чтения %s", name,buffer);
				strcpy(buffer,"unknown give-me");
			}
			return true;
		}
		if(isSetYouCommand(buffer)){
			if (!doSetCommand(buffer)) {
				Debug_Message(LOG_ERROR, "%s Неизвестная команда записи %s", name,buffer);
				strcpy(buffer,"unknown set-you");
			} else strcpy(buffer,"ok");
			return true;
		}
		Debug_Message(LOG_ERROR, "%s Неизвестная команда %s", name,buffer);
		strcpy(buffer,"bad message");
		return true;
}
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
	StatusSet ss;
	GetCopy(StatusSetName,&ss);
	ss.sServer=1;
	ss.motiv=1;
	SetCopy(StatusSetName,&ss);
	if (socket < 0) return;
	int err=shutdown(socket, SHUT_RDWR);
	if(err!=0) {
		Debug_Message(LOG_ERROR, "shutdown %d",errno);
	}
	err=close(socket);
	if(err!=0) {
		Debug_Message(LOG_ERROR, "close socket %d",errno);
	}
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
