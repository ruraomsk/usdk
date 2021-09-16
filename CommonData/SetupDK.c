/*
 * SetDK.c
 *
 *  Created on: 14 сент. 2021 г.
 *      Author: rura
 */

#include <stdlib.h>
#include "CommonData.h"
#include "core_json.h"

void clearSetupDK(SetupDK *setupDK) {
	setupDK->dkn = 10234;
	setupDK->tmaxf = 8;
	setupDK->tminf = 5;
	setupDK->tminmax = 7;
	setupDK->dktype = 10;
	setupDK->extn = 15678;
	setupDK->tprom = 6;
	setupDK->preset = false;
}

char* SetupDKToJsonString(SetupDK *setupDK){
	js_write jswork;
	js_write_start(&jswork, 256);
	js_write_int(&jswork, "dkn", setupDK->dkn);
	js_write_int(&jswork, "tmaxf", setupDK->tmaxf);
	js_write_int(&jswork, "tminf", setupDK->tminf);
	js_write_int(&jswork, "tminmax", setupDK->tminmax);
	js_write_int(&jswork, "dktype", setupDK->dktype);
	js_write_int(&jswork, "extn", setupDK->extn);
	js_write_int(&jswork, "tprom", setupDK->tprom);
	js_write_bool(&jswork, "preset", setupDK->preset);
	js_write_end(&jswork);
	return jswork.start;
}
void SetupDKFromJsonString(char *root, SetupDK *setupDK) {
	js_read jswork;
	char str[60];
	double d;
	js_read_start(&jswork, root);
	js_read_int(&jswork, "dkn", &setupDK->dkn);
	js_read_int(&jswork, "tmaxf", &setupDK->tmaxf);
	js_read_int(&jswork, "tminf", &setupDK->tminf);
	js_read_int(&jswork, "tminmax", &setupDK->tminmax);
	js_read_int(&jswork, "dktype", &setupDK->dktype);
	js_read_int(&jswork, "extn", &setupDK->extn);
	js_read_int(&jswork, "tprom", &setupDK->tprom);
	js_read_bool(&jswork, "preset", &setupDK->preset);
}

