/*
 * GPSSet.c
 *
 *  Created on: Oct 4, 2021
 *      Author: rura
 */
#include "CommonData.h"
#include <stdlib.h>
#include "core_json.h"

void clearGPSSet(GPSSet *g){
	g->Ok=false;
	g->E01=false;
	g->E02=false;
	g->E03=false;
	g->E04=false;
	g->Seek=false;
}
char* GPSSetToJsonString(GPSSet *g,char* buffer,size_t size){
	js_write jswork;
	if (buffer==NULL){
		js_write_start(&jswork, size);
	} else {
		js_write_static(&jswork,buffer,size);
	}
	js_write_bool(&jswork, "Ok", g->Ok);
	js_write_bool(&jswork, "E01", g->E01);
	js_write_bool(&jswork, "E02", g->E02);
	js_write_bool(&jswork, "E03", g->E03);
	js_write_bool(&jswork, "E04", g->E04);
	js_write_bool(&jswork, "Seek", g->Seek);
	js_write_end(&jswork);
	return jswork.start;

}
void GPSSetFromJsonString(char *root, GPSSet *g){
	js_read jswork;
	js_read_start(&jswork, root);
	js_read_bool(&jswork, "Ok", &g->Ok);
	js_read_bool(&jswork, "E01", &g->E01);
	js_read_bool(&jswork, "E02", &g->E02);
	js_read_bool(&jswork, "E03", &g->E03);
	js_read_bool(&jswork, "E04", &g->E04);
	js_read_bool(&jswork, "Seek", &g->Seek);
}
