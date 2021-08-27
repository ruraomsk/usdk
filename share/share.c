/*
 * share.c
 *
 *  Created on: Aug 11, 2021
 *      Author: rura
 */

#include "share.h"
#include "parson.h"
#include <search.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "DebugLogger.h"
static char *charts[] = { "debug", "tcpconnect", "setupdevice", "transport" };

static char *values[] = { "{\"ip\":\"192.168.115.159\",\"port\":2095}",
		"{\"ip\":\"192.168.115.159\",\"port\":2093,\"timeoutread\":10,\"timeoutwrite\":1,\"timeoutconn\":5}",
		"{\"id\":\true,\"eth\":\true,\"gprs\":true,\"gps\":true,\"usb\":true,\"modbusslave\":true,\"modbusmaster\":true}",
		"{}" };
static osMutexId_t ShareMutex;

extern int ReadyShare;
static char ShareMessage[256];

static void* share_malloc (unsigned int size){
	void* result=malloc(size);
	return result;
}
static void share_free(void* buffer){
	free(buffer);
}

void ShareSaveChange() {
	int count=0;
	if (osMutexAcquire(ShareMutex, osWaitForever) == osOK) {
		int size=sizeof(charts)/sizeof(charts[0]);
		for (size_t i = 0; i < size; i++) {
			ENTRY e, *ep;
//			ShareValue *value;
			e.key = charts[i];
			ep = hsearch(e, FIND);
			if (ep != NULL) {
				if (((ShareValue*) ep->data)->changed) {
					JSON_Value *value = ((ShareValue*) ep->data)->json_root;
//					char *string =
					json_serialize_to_buffer(value, ShareMessage, 80);
//					Debug_Message(LOG_INFO, ShareMessage);
					// Save to EEPROM
					count++;
					((ShareValue*) ep->data)->changed=0;
					e.data=ep->data;
					hsearch(e, ENTER);
//					sprintf(ShareMessage, "save json %s", charts[i]);
//					Debug_Message(LOG_INFO, ShareMessage);
				}
			} else {
				sprintf(ShareMessage, "Раздел %s испорчен", charts[i]);
				Debug_Message(LOG_INFO, ShareMessage);
			}
		}
	}
	osMutexRelease(ShareMutex);
	if(count) {
		Debug_Message(LOG_INFO, "share сохранено");
	}
	else{
//		Debug_Message(LOG_INFO, "share не изменялось");
	}
}
void ShareInit() {
	ENTRY e, *ep;
	ShareValue *value;
	ShareMutex=osMutexNew(NULL);
	json_set_allocation_functions(share_malloc, share_free);
	const size_t capacity = sizeof charts / sizeof charts[0];
	hcreate(capacity);
	for (size_t i = 0; i < capacity; i++) {
		value = malloc(sizeof(ShareValue));
		e.key = charts[i];
		//Load string
//		sprintf(ShareMessage, "string json %s", values[i]);
//		Debug_Message(LOG_INFO, ShareMessage);
		value->json_root = json_parse_string(values[i]);
		if(value->json_root!=NULL){
//			json_serialize_to_buffer(value->json_root, &ShareMessage, 120);
//			Debug_Message(LOG_INFO, ShareMessage);
//			sprintf(ShareMessage, "load json %s", charts[i]);
//			Debug_Message(LOG_INFO, ShareMessage);
			value->changed = 0;
			e.data = (void*) value;
			ep = hsearch(e, ENTER);
			if (ep == NULL) {
				Debug_Message(LOG_FATAL, "Ошибка словаря");
				return;
			}
		}
	}
	ReadyShare = 1;
	Debug_Message(LOG_INFO, "Share загружена");
}


JSON_Value* ShareGetJson(const char *chart) {
	JSON_Value *result = NULL;
	if (osMutexAcquire(ShareMutex, osWaitForever) == osOK) {
		ENTRY e, *ep;
//		ShareValue *value;
		e.key = chart;
		ep = hsearch(e, FIND);
		if (ep != NULL) {
			result = ((ShareValue*) ep->data)->json_root;
		}
		osMutexRelease(ShareMutex);
	}
	return result;
}
void ShareSetJson(const char *chart, JSON_Value *rvalue) {
	if (osMutexAcquire(ShareMutex, osWaitForever) == osOK) {
		ENTRY e, *ep;
		ShareValue *value;
		e.key = chart;
		ep = hsearch(e, FIND);
		if (ep != NULL) {
			free(ep->data);
			value = malloc(sizeof(ShareValue));
			value->changed = 1;
			value->json_root = rvalue;
			e.data = (void*) value;
			if (hsearch(e, ENTER) == NULL)
				Debug_Message(LOG_FATAL, "Ошибка словаря");
		}
		osMutexRelease(ShareMutex);
	}
}
