/*
 * service.c
 *
 *  Created on: 28 сент. 2021 г.
 *      Author: rura
 */
#include <string.h>
#include "service.h"
#include "CommonData.h"
#include "Common_private.h"
#include "DeviceTime.h"
#include "DebugLogger.h"
#include <sockets.h>
#include "Transport.h"
char confirm[]="confirm=good 012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789";
//char confirm [ ] = "confirm=good";

int Interval = 10;
int tout = 10;
#define MESSAGE_OK "ok"
#define GIVE_ME_STATUS "give_me_status"
#define DIFF_INTERVAL 2
extern GPSSet gpsSet;

char* MessageConfirm(char* buffer) {
	buffer [ 0 ] = 0;
	strcat(buffer, confirm);
	return buffer;
}

char* MessageStatusDevice(char*buffer) {
	*buffer='\0';
	strcat(buffer, "state=");
	NowState ns;
	GetCopy(NowStateName, &ns);
	NowStateToJsonString(&ns,buffer+strlen(buffer), 2000);
	return buffer;
}
#define TCP_WINDOW 2400
char endln [ ] = "\n";
int sendString(int socket, void *buffer, size_t len) {
	int count = 0;
	while (len > 0) {
		int l = len > TCP_WINDOW ? TCP_WINDOW : len;
		//Устанавливаем тайм ауты
		struct timeval tv = { 0, 0 };
		tv.tv_sec = 5;
		int err = lwip_setsockopt(socket, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
		if (err!=0) {
			Debug_Message(LOG_ERROR, "Тайм аут на передачу %d",err);
		}
		int f = send(socket, buffer, l, 0);
//		Debug_Message(LOG_INFO, "send %d",f);
		if (f != l) return -1;
		buffer += l;
		len -= l;
		count += l;
//		osDelay(STEP_CONTROL);
	}
	//Устанавливаем тайм ауты
	struct timeval tv = { 0, 0 };
	tv.tv_sec = 5;
	int err = lwip_setsockopt(socket, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
	if (err!=0) {
		Debug_Message(LOG_ERROR, "Тайм аут на передачу %d",err);
	}
	int f = send(socket, endln, strlen(endln), 0);
	if (f != 1) return -1;
	return ++count;
}
int readString(int socket, char *buffer, size_t len, int timeout) {
	char *start = buffer;
	time_t tstart = GetDeviceTime();
	int count = 0;
	while (len > 0) {
		int l = len > TCP_WINDOW ? TCP_WINDOW : len;
		//Устанавливаем тайм ауты
		struct timeval tv = { 0, 0 };
		tv.tv_sec = 5;
		int err = lwip_setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
		if (err!=0) {
			Debug_Message(LOG_ERROR, "Тайм аут на чтение %d",err);
		}
		int f = read(socket, buffer, l);
//		Debug_Message(LOG_INFO, "read %d",f);
		if (f < 1) {
			if (DiffTimeSecond(tstart) > timeout) return count;
			osDelay(STEP_CONTROL);
			continue;
		}
		if (buffer [ f - 1 ] == '\n') {
			buffer [ f - 1 ] = 0;
			return count + f;
		}
		count += f;
		buffer += f;
		len -= f;
	}
	start [ count - 1 ] = 0;
	return count;
}
void setTimeoutForChanel(int interval){
	TCPSet tcpSetSec;
	TCPSet tcpSetMain;
	GetCopy(TCPSetSecName, &tcpSetSec);
	GetCopy(TCPSetMainName, &tcpSetMain);
	tcpSetSec.tque=interval+DIFF_INTERVAL;
	tcpSetMain.tque=interval+DIFF_INTERVAL;
	SetCopy(TCPSetMainName, &tcpSetMain);
	SetCopy(TCPSetSecName, &tcpSetSec);
}

void prepareConnectMessage(char *message) {
	int server_number, interval;
	sscanf(message + sizeof(MESSAGE_OK), "%d,%d", &server_number, &interval);
	if (interval != Interval) {
		Interval = interval;
		setTimeoutForChanel(Interval);
	}
}
void prepareGiveMeStatus(char *message){
	char *end;
	time_t newTime=strtoul(message+sizeof(GIVE_ME_STATUS), &end, 10);
	if (!gpsSet.Ok)	UpdateDeviceTime(newTime);
}
bool isGive_Me_Status(char *message) {
	if (strncmp(GIVE_ME_STATUS, message, strlen(GIVE_ME_STATUS)) == 0) return true;
	return false;
}
bool isConnect(char *message) {
	if (strncmp(MESSAGE_OK, message, strlen(MESSAGE_OK)) == 0) return true;
	return false;
}

