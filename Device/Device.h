/*
 * Device.h
 *
 *  Created on: 27 авг. 2021 г.
 *      Author: rura
 */

#ifndef DEVICE_H_
#define DEVICE_H_

#define true 1
#define false 0

#define DeviceBufferSize 1024
#define MAX_LEN_TCP_MESSAGE 2048

typedef struct {
	int error;
	char *message;
} MessageFromQueue;

typedef struct {
	int  ID;
	char Ethertnet;
	char Gpgrs;
	char Gps;
	char Usb;
} DeviceStatus;


DeviceStatus readSetup(DeviceStatus* deviceStatus,const char *name);
void writeSetup(const char *name,const DeviceStatus* deviceStatus);
void makeConnectString(char *buffer,const size_t buffersize,const char *typestring,const DeviceStatus* deviceStatus);

#endif /* DEVICE_H_ */
