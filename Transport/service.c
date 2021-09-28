/*
 * service.c
 *
 *  Created on: 28 сент. 2021 г.
 *      Author: rura
 */

#include "service.h"

char* MessageConfirm (){
	js_write w;
	js_write_start(&w, 120);
	js_write_value_start(&w, "confirm");
	js_write_bool(&w, "status", true);
	js_write_bool(&w, "ready", true);
	js_write_string(&w, "error", "No error");
	js_write_value_end(&w);
	js_write_end(&w);
	return w.start;
}

char* MessageStatusDevice(){
	// @formatter:off
	js_write w;
	js_write_start(&w, 1024);
	js_write_value_start(&w, "status");
	js_write_bool(&w, "ready", true);
	js_write_int(&w, "tech", 1);
	js_write_bool(&w, "base", true);
	js_write_int(&w, "pk", 1);
	js_write_int(&w, "ck", 1);
	js_write_int(&w, "nk", 1);
	js_write_int(&w, "tmax", 1);

	js_write_value_start(&w, "comdu");
		js_write_bool(&w,"IsPK",true);
		js_write_bool(&w,"IsCK",true);
		js_write_bool(&w,"IsNK",true);
		js_write_bool(&w,"IsDUDK1",true);
		js_write_bool(&w,"IsDUDK2",true);
		js_write_bool(&w,"IsReqSFDK1",true);
		js_write_bool(&w,"IsReqSFDK2",true);
	js_write_value_end(&w);

	js_write_value_start(&w, "dk");
		js_write_int(&w,"rdk",1);
		js_write_int(&w,"fdk",2);
		js_write_int(&w,"ddk",3);
		js_write_int(&w,"edk",4);
		js_write_bool(&w,"pdk",true);
		js_write_int(&w,"eedk",4);
		js_write_bool(&w,"odk",true);
		js_write_int(&w,"ldk",5);
		js_write_int(&w,"ftudk",4);
		js_write_int(&w,"tdk",6);
		js_write_int(&w,"ftsdk",5);
		js_write_int(&w,"ttcdk",5);
	js_write_value_end(&w);

	js_write_value_end(&w);
	js_write_end(&w);
	return w.start;
	// @formatter:on

}
