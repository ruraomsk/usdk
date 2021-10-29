/*
 * getset.h
 *
 *  Created on: 19 окт. 2021 г.
 *      Author: rura
 */

#ifndef GETSET_H_
#define GETSET_H_

#include <stdbool.h>

char* give_me_Status(char *buffer);
char* give_me_GPS(char *buffer);
char* give_me_Error(char *buffer);
char* give_me_SetupDK(char *buffer);
bool set_you_SetupDK(char *buffer);
char* give_me_Cameras(char *buffer);
bool set_you_Cameras(char *buffer);
char* give_me_TCPMain(char *buffer);
bool set_you_TCPMain(char *buffer);
char* give_me_TCPSec(char *buffer);
bool set_you_TCPSec(char *buffer);
char* give_me_OneMonth(char *buffer, int param);
bool set_you_OneMonth(char *buffer, int param);
char* give_me_OneWeek(char *buffer, int param);
bool set_you_OneWeek(char *buffer, int param);
char* give_me_OneDay(char *buffer, int param);
bool set_you_OneDay(char *buffer, int param);
char* give_me_OnePK(char *buffer, int param);
bool set_you_OnePK(char *buffer, int param);
char* give_me_OnePhase(char *buffer, int param);
bool set_you_OnePhase(char *buffer, int param);


#endif /* GETSET_H_ */
