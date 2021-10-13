/*
 * CameraSet.c
 *
 *  Created on: Oct 5, 2021
 *      Author: rura
 */

#include <stdlib.h>
#include "core_json.h"
#include "CommonData.h"

void clearCameraSet(CameraSet *c) {
	OneCameraConn empty={.ip="192.168.115.168",.port=8441,.id=12,.login="admin",.password="admin",.chanels=8};
//	c->cameras[0] = empty;
	for (int i = 0; i < MAX_CAMERAS; ++i) {
		if (i<MAX_CAMERAS) empty.id=i+1;
		else empty.id=0;//i+1
		c->cameras[i] = empty;
	}
}
char* CameraSetToJsonString(CameraSet *c,char* buffer,size_t size) {
	js_write jswork;
	if (buffer==NULL){
		js_write_start(&jswork, size);
	} else {
		js_write_static(&jswork,buffer,size);
	}

	js_write_array_start(&jswork, "cameras");
	for (int i = 0; i < MAX_CAMERAS ; ++i) {
		js_write_value_start(&jswork, "");
		js_write_int(&jswork, "id",c->cameras[i].id);
		js_write_string(&jswork, "ip",c->cameras[i].ip);
		js_write_string(&jswork, "login",c->cameras[i].login);
		js_write_string(&jswork, "password",c->cameras[i].password);
		js_write_int(&jswork, "port",c->cameras[i].port);
		js_write_int(&jswork, "chanels",c->cameras[i].chanels);
		js_write_value_end(&jswork);
	}
	js_write_array_end(&jswork);
	js_write_end(&jswork);
	return jswork.start;
}
void CameraSetFromJsonString(char *root, CameraSet *c) {
	js_read jswork;
	js_read jcameras;
	js_read objcamera;
	js_read_start(&jswork, root);
	js_read_array(&jswork, &jcameras, "cameras");
	for (int i = 0; i < MAX_CAMERAS; ++i) {
		if (js_read_array_object(&jcameras, i, &objcamera)!=JsonSuccess) break;
		js_read_int(&objcamera, "id",&c->cameras[i].id);
		js_read_string(&objcamera, "ip",c->cameras[i].ip);
		js_read_string(&objcamera, "login",c->cameras[i].login);
		js_read_string(&objcamera, "password",c->cameras[i].password);
		js_read_int(&objcamera, "port",&c->cameras[i].port);
		js_read_int(&objcamera, "chanels",&c->cameras[i].chanels);
	}
}

