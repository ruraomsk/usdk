/*
 * StatusSet.c
 *
 *  Created on: 19 окт. 2021 г.
 *      Author: rura
 */

#include "CommonData.h"
#include <stdlib.h>
#include "core_json.h"
#include <string.h>



void clearStatusSet(StatusSet *s){
	memset(s,0,sizeof(StatusSet));
}
char* StatusSetToJsonString(StatusSet *s,char* buffer,size_t size){
	js_write jswork;
	if (buffer==NULL){
		js_write_start(&jswork, size);
	} else {
		js_write_static(&jswork,buffer,size);
	}
	js_write_int(&jswork, "s220", s->s220);
	js_write_int(&jswork, "sGPS", s->sGPS);
	js_write_int(&jswork, "sServer", s->sServer);
	js_write_int(&jswork, "sPSPD", s->sPSPD);
	js_write_int(&jswork, "elc", s->elc);
	js_write_bool(&jswork, "ethernet", s->ethernet);
	js_write_int(&jswork, "tobm", s->tobm);
	js_write_int(&jswork, "lnow", s->lnow);
	js_write_int(&jswork, "llast", s->llast);
	js_write_int(&jswork, "motiv", s->motiv);
	js_write_end(&jswork);
	return jswork.start;

}
void StatusSetFromJsonString(char *root, StatusSet *s){
	js_read jswork;
	js_read_start(&jswork, root);
	js_read_int(&jswork, "s220", &s->s220);
	js_read_int(&jswork, "sGPS", &s->sGPS);
	js_read_int(&jswork, "sServer", &s->sServer);
	js_read_int(&jswork, "sPSPD", &s->sPSPD);
	js_read_int(&jswork, "elc", &s->elc);
	js_read_bool(&jswork, "ethernet", &s->ethernet);
	js_read_int(&jswork, "tobm", &s->tobm);
	js_read_int(&jswork, "lnow", &s->lnow);
	js_read_int(&jswork, "llast", &s->llast);
	js_read_int(&jswork, "motiv", &s->motiv);
}
