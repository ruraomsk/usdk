/*
 * Shell.h
 *
 *  Created on: 12 нояб. 2021 г.
 *      Author: rura
 */

#ifndef SHELL_H_
#define SHELL_H_

#include <stdint.h>
#include "usbd_cdc_if.h"

void InitShell(void);
void ShellWork(void* arg);
void doCmd(char* buffer);
void cmdInit(void);


void sayReady(char* buffer);
void writeDataToUsb(char* buffer,uint16_t lenght);

void dircommand(char* path,char* buffer);
void cdcommand(char* path,char* newpath,char* buffer);
void sendFile(char* path,char* filename,char* buffer);
#endif /* SHELL_H_ */
