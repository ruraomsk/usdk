/*
 * service.h
 *
 *  Created on: 28 сент. 2021 г.
 *      Author: rura
 */

#ifndef SERVICE_H_
#define SERVICE_H_

#include "core_json.h"
#include <stdbool.h>

char* MessageConfirm (char *buffer);
char* MessageStateDevice(char *buffer);
int sendString(int socket,void* buffer,size_t len);
int readString(int socket,char* buffer,size_t len,int timeout);
bool isConnect(char* message);
bool isGive_Me_State(char* message);
void prepareGiveMeState(char *message);
void setTimeoutForChanel(int interval);
#endif /* SERVICE_H_ */
