/*
 * Transport.h
 *
 *  Created on: 30 авг. 2021 г.
 *      Author: rura
 */

#ifndef TRANSPORT_H_
#define TRANSPORT_H_

#include "cmsis_os.h"
#include <stddef.h>


#define true 1
#define false 0

#define STEP_TCP 				3000
#define STEP_GPRS 				3000

#define DeviceBufferSize 1024
#define MAX_LEN_TCP_MESSAGE 2048
#define STEP_CONTROL 100
enum {
	TRANSPORT_OK=0,
	TRANSPORT_ERROR,
	TRANSPORT_STOP,
	TRANSPORT_KEEP_ALIVE
};
typedef struct {
	int error;
	char *message;
} MessageFromQueue;


typedef struct {
	int  ID;
	char Ethertnet;
	char Gprs;
	char Gps;
	char Usb;
} DeviceStatus;

void mainTransportLoop(void);

DeviceStatus readSetup( char *name);
void writeSetup(char *name,const DeviceStatus* deviceStatus);
void makeConnectString(char *buffer,const size_t buffersize,const char *typestring,const DeviceStatus* deviceStatus);
void BadTCP(char *buffer,int socket,osMessageQueueId_t que);
void BadGPRS(char *buffer,int socket,osMessageQueueId_t que);
void setGoodTCP(int v);
void setGoodGPRS(int v);
int  isGoodTCP(void);
int  isGoodGPRS(void);
void setFromServerTCPStart(int v);
void setFromServerGPRSStart(int v);
void setGPRSNeed(int v);


void FromServerTCPLoop(void);
void ToServerTCPLoop(void);
void FromServerGPRSLoop(void);
void ToServerGPRSLoop(void);
#endif /* TRANSPORT_H_ */
