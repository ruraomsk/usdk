/*
 * DeviceStatus.c
 *
 *  Created on: Sep 17, 2021
 *      Author: rura
 */


#include <stdlib.h>
#include "CommonData.h"
#include "core_json.h"

void clearDeviceStatus(DeviceStatus *ds) {
	ds->ID=8;
	ds->Ethertnet=true;
	ds->Gprs=false;
	ds->Gps=true;
	ds->Usb=true;
	ds->Camera=false;
}

char* DeviceStatusToJsonString(DeviceStatus *ds,char* buffer,size_t size){
	js_write jswork;
	if (buffer==NULL){
		js_write_start(&jswork, size);
	} else {
		js_write_static(&jswork,buffer,size);
	}
	js_write_int(&jswork, "id", ds->ID);
	js_write_bool(&jswork, "eth",  ds->Ethertnet);
	js_write_bool(&jswork, "gprs",  ds->Gprs);
	js_write_bool(&jswork, "gps",  ds->Gps);
	js_write_bool(&jswork, "usb",  ds->Usb);
	js_write_bool(&jswork, "cam",  ds->Camera);
	js_write_end(&jswork);
	return jswork.start;
}
void DeviceStatusFromJsonString(char *root, DeviceStatus *ds) {
	js_read jswork;
	js_read_start(&jswork, root);
	js_read_int(&jswork, "id", &ds->ID);
	js_read_bool(&jswork, "eth", &ds->Ethertnet);
	js_read_bool(&jswork, "gprs", &ds->Gprs);
	js_read_bool(&jswork, "gps", &ds->Gps);
	js_read_bool(&jswork, "usb", &ds->Usb);
	js_read_bool(&jswork, "cam", &ds->Camera);
}
