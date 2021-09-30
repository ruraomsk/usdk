/*
 * TCPSet.c
 *
 *  Created on: Sep 17, 2021
 *      Author: rura
 */


#include <stdlib.h>
#include <string.h>
#include "CommonData.h"
#include "core_json.h"

void clearTCPSet(TCPSet *t) {
	strcpy(t->ip,"192.168.115.159");
	t->port=2093;
	t->tread=1;
	t->twrite=1;
	t->tque=30;
}

char* TCPSetToJsonString(TCPSet *t,size_t size){
	js_write jswork;
	js_write_start(&jswork, size);
	js_write_int(&jswork, "port", t->port);
	js_write_int(&jswork, "tread", t->tread);
	js_write_int(&jswork, "twrite", t->twrite);
	js_write_int(&jswork, "tque", t->tque);
	js_write_string(&jswork, "ip",  t->ip);
	js_write_end(&jswork);
	return jswork.start;
}
void TCPSetFromJsonString(char *root, TCPSet *t) {
	js_read jswork;
	js_read_start(&jswork, root);
	js_read_int(&jswork, "port", &t->port);
	js_read_int(&jswork, "tread", &t->tread);
	js_read_int(&jswork, "twrite", &t->twrite);
	js_read_int(&jswork, "tque", &t->tque);
	js_read_string(&jswork, "ip",  t->ip);
}
