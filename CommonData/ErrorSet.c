/*
 * ErrorSet.c
 *
 *  Created on: 19 окт. 2021 г.
 *      Author: rura
 */

#include "CommonData.h"
#include <stdlib.h>
#include "core_json.h"
#include <string.h>



void clearErrorSet(ErrorSet *e){
	memset(e,0,sizeof(ErrorSet));
}
char* ErrorSetToJsonString(ErrorSet *e,char* buffer,size_t size){
	js_write jswork;
	if (buffer==NULL){
		js_write_start(&jswork, size);
	} else {
		js_write_static(&jswork,buffer,size);
	}
	js_write_bool(&jswork, "V220DK1", e->V220DK1);
	js_write_bool(&jswork, "V220DK2", e->V220DK2);
	js_write_bool(&jswork, "RTC", e->DRTC);
	js_write_bool(&jswork, "TVP1", e->DTVP1);
	js_write_bool(&jswork, "TVP2", e->DTVP2);
	js_write_bool(&jswork, "FRAM", e->DFRAM);
	js_write_end(&jswork);
	return jswork.start;

}
void ErrorSetFromJsonString(char *root, ErrorSet *e){
	js_read jswork;
	js_read_start(&jswork, root);
	js_read_bool(&jswork, "V220DK1", &e->V220DK1);
	js_read_bool(&jswork, "V220DK2", &e->V220DK2);
	js_read_bool(&jswork, "RTC", &e->DRTC);
	js_read_bool(&jswork, "TVP1", &e->DTVP1);
	js_read_bool(&jswork, "TVP2", &e->DTVP2);
	js_read_bool(&jswork, "FRAM", &e->DFRAM);
}

