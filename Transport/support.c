/*
 * ReadWrite.c
 *
 *  Created on: 27 авг. 2021 г.
 *      Author: rura
 */

#include <sockets.h>
#include <stdlib.h>

#include "Transport.h"
#include "parson.h"
#include "Files.h"

extern int TCPError;
extern int GPRSError;
extern int GPRSNeed;			//Готовность работы GPRS если есть
extern int FromServerTCPStart;
extern int FromServerGPRSStart;

extern osMutexId_t TransportMutex;
DeviceStatus readSetup(char *name) {
	DeviceStatus d;
	JSON_Value *root = ShareGetJson(name);
	if (root==NULL){
		printf("ERROR");
	}
	JSON_Object *object = json_value_get_object(root);
	d.ID = (int) json_object_get_number(object, "id");
	d.Ethertnet = (char) json_object_get_boolean(object, "eth");
	d.Gprs = (char) json_object_get_boolean(object, "gprs");
	d.Gps = (char) json_object_get_boolean(object, "gps");
	d.Usb = (char) json_object_get_boolean(object, "usb");
	return d;
}

void writeSetup(char *name,const DeviceStatus* deviceStatus) {
	JSON_Value *root_value = json_value_init_object();
	JSON_Object *root_object = json_value_get_object(root_value);
	json_object_set_number (root_object, "id", deviceStatus->ID);
	json_object_set_boolean(root_object, "eth", deviceStatus->Ethertnet);
	json_object_set_boolean(root_object, "gprs", deviceStatus->Gprs);
	json_object_set_boolean(root_object, "gps", deviceStatus->Gps);
	json_object_set_boolean(root_object, "usb", deviceStatus->Gps);
	ShareSetJson(name, root_value);
}
void makeConnectString(char *buffer,const size_t buffersize,const char *typestring,const DeviceStatus* deviceStatus){
	JSON_Value *root_value = json_value_init_object();
	JSON_Object *root_object = json_value_get_object(root_value);
	json_object_set_number (root_object, "id", deviceStatus->ID);
	json_object_set_string(root_object, "type", typestring);
	json_serialize_to_buffer(root_value, buffer, buffersize);
}
void setFromServerTCPStart(int v){
	if (osMutexAcquire(TransportMutex, osWaitForever)==osOK){
		FromServerTCPStart=v;
		osMutexRelease(TransportMutex);
	}
}
void setFromServerGPRSStart(int v){
	if (osMutexAcquire(TransportMutex, osWaitForever)==osOK){
		FromServerGPRSStart=v;
		osMutexRelease(TransportMutex);
	}
}
void setGPRSNeed(int v){
	if (osMutexAcquire(TransportMutex, osWaitForever)==osOK){
		GPRSNeed=v;
		osMutexRelease(TransportMutex);
	}
}
void setGoodTCP(int v){
	if (osMutexAcquire(TransportMutex, osWaitForever)==osOK){
		TCPError=v;
		osMutexRelease(TransportMutex);
	}
}
void setGoodGPRS(int v){
	if (osMutexAcquire(TransportMutex, osWaitForever)==osOK){
		GPRSError=v;
		osMutexRelease(TransportMutex);
	}
}
int isGoodTCP(){
	return TCPError;
}
int isGoodGPRS(){
	return GPRSError;
}
void BadTCP(char *buffer,int socket,osMessageQueueId_t que) {
	setGoodTCP(0);
	free(buffer);
	MessageFromQueue msg;
	msg.error = TRANSPORT_ERROR;
	msg.message = NULL;
	osMessageQueuePut(que, &msg, 0, 0);
	if(socket<0) return;
	shutdown(socket, SHUT_RDWR);
	close(socket);
	osDelay(1000);
}
void BadGPRS(char *buffer,int socket,osMessageQueueId_t que) {
	setGoodGPRS(0);
	free(buffer);
	MessageFromQueue msg;
	msg.error = TRANSPORT_ERROR;
	msg.message = NULL;
	osMessageQueuePut(que, &msg, 0, 0);
	if(socket<0) return;
	shutdown(socket, SHUT_RDWR);
	close(socket);
	osDelay(1000);
}
