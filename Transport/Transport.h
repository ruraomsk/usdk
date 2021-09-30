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
#include <stdbool.h>


#define STEP_TCP 				3000
#define STEP_GPRS 				3000

#define DeviceBufferSize 1024
#define MAX_LEN_TCP_MESSAGE 1024
#define STEP_CONTROL 100

#define SIGNAL_NEED_KEEP_ALIVE 1

typedef enum {
	TRANSPORT_OK=0,
	TRANSPORT_ERROR,
	TRANSPORT_STOP,
	TRANSPORT_KEEP_ALIVE
}ErrorTransport;
typedef struct {
	ErrorTransport error;
	char *message;
} MessageFromQueue;

void mainTransportLoop(void);

char* makeConnectString(const size_t buffersize,char *typestring);
void BadTCP(char *buffer,int socket,osMessageQueueId_t que);
void BadGPRS(char *buffer,int socket,osMessageQueueId_t que);
void setGoodTCP(bool v);
void setGoodGPRS(bool v);
bool  isGoodTCP(void);
bool  isGoodGPRS(void);
void setToServerTCPStart(bool v);
void setToServerGPRSStart(bool v);
void setGPRSNeed(bool v);

void setTimeoutForChanel(int interval);

void deleteEnter(void *buffer);
void TechExchange(void *argument);
void FromServerTCPLoop(void);
void ToServerTCPLoop(void);
void FromServerGPRSLoop(void);
void ToServerGPRSLoop(void);
#endif /* TRANSPORT_H_ */
