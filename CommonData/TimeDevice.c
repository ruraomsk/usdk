/*
 * TimeDevice.c
 *
 *  Created on: Oct 4, 2021
 *      Author: rura
 */


#include <stdlib.h>
#include <string.h>
#include "CommonData.h"
#include "core_json.h"

void clearTimeDevice(TimeDevice *t) {
	t->TimeZone=6;
	t->Summer=false;
}

char* TimeDeviceToJsonString(TimeDevice *t,size_t size){
	js_write jswork;
	js_write_start(&jswork, size);
	js_write_int(&jswork, "tz", t->TimeZone);
	js_write_int(&jswork, "summer", t->Summer);
	js_write_end(&jswork);
	return jswork.start;
}
void TimeDeviceFromJsonString(char *root, TimeDevice *t) {
	js_read jswork;
	js_read_start(&jswork, root);
	js_read_int(&jswork, "tz", &t->TimeZone);
	js_read_bool(&jswork, "summer", &t->Summer);
}
