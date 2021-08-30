/*
 * ReadWrite.c
 *
 *  Created on: 27 авг. 2021 г.
 *      Author: rura
 */

#include "Transport.h"
#include "parson.h"

DeviceStatus readSetup(const char *name) {
	DeviceStatus d;
	JSON_Value *root = ShareGetJson(name);
	JSON_Object *object = json_value_get_object(root);
	d.ID = (int) json_object_get_number(object, "id");
	d.Ethertnet = (char) json_object_get_boolean(object, "eth");
	d.Gprs = (char) json_object_get_boolean(object, "gprs");
	d.Gps = (char) json_object_get_boolean(object, "gps");
	d.Usb = (char) json_object_get_boolean(object, "usb");
}

void writeSetup(const char *name,const DeviceStatus* deviceStatus) {
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
