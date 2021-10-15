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
#include "service.h"
#include "CommonData.h"

#define STEP_TCP 				1000
#define STEP_GPRS 				20000

#define DeviceBufferSize 2400
#define MAX_LEN_TCP_MESSAGE 12400
#define STEP_CONTROL 100

#define LenConnectString 100


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

void TransportStart(void);

char* makeConnectString(char *typestring);


void BadTCP(int socket);
void BadGPRS(char *buffer,int socket,osMessageQueueId_t que);
void setGoodTCP(bool v);
void setGoodGPRS(bool v);
bool  isGoodTCP(void);
bool  isGoodGPRS(void);
void setToServerTCPStart(bool v);
void setToServerGPRSStart(bool v);
void setGPRSNeed(bool v);

char* doGiveCommand(char* buffer);
bool doSetCommand(char *buffer);
bool doControlCommand(char* buffer);

bool isGiveMeCommand(char *buffer);
bool isSetYouCommand(char *buffer);
bool isControlCommand(char *buffer);

bool MakeReplay(bool *conn,char* buffer,char* name);
void setTimeoutForChanel(int interval);

void prepareConnectMessage(char* message);

void deleteEnter(char* buffer);
void FromServerTCPLoop(void);
void ToServerTCPLoop(void);
void FromServerGPRSLoop(void);
void ToServerGPRSLoop(void);

bool ControlData(char *buffer, YearSet *ys, WeekSet *ws, DaySet *ds, AllPks *pks, PhasesSet *ps);

#endif /* TRANSPORT_H_ */
