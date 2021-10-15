/*
 * SetPk.c
 *
 *  Created on: Oct 6, 2021
 *      Author: rura
 */

#include <stdlib.h>
#include <string.h>
#include "core_json.h"
#include "CommonData.h"

void clearAllPks(AllPks *apks) {
	memset(apks,0,sizeof(AllPks));
	for (int p 	= 0; p < MAX_PKS; ++p) {
		apks->pks[p].pk=p+1;
		for (int s = 0; s < MAX_STAGES; ++s) {
			apks->pks[p].stages[s].line=s+1;
		}
	}
}
char* AllPksToJsonString(AllPks *apks,char* buffer,size_t size) {
	js_write jswork;
	if (buffer==NULL){
		js_write_start(&jswork, size);
	} else {
		js_write_static(&jswork,buffer,size);
	}
	js_write_array_start(&jswork, "dk");
	for (int i = 0; i < MAX_PKS ; ++i) {
		js_write_value_start(&jswork, "");
		js_write_int(&jswork, "pk",apks->pks[i].pk );
		js_write_int(&jswork, "tpu",apks->pks[i].tpu);
		js_write_bool(&jswork, "razlen",apks->pks[i].razlen);
		js_write_int(&jswork, "tc",apks->pks[i].tc);
		js_write_int(&jswork, "shift",apks->pks[i].shift);
		js_write_bool(&jswork, "twot",apks->pks[i].twot);
		js_write_array_start(&jswork, "sts");
		for (int j = 0; j < MAX_STAGES; ++j) {
			js_write_value_start(&jswork, "");
			js_write_int(&jswork, "line",apks->pks[i].stages[j].line );
			js_write_int(&jswork, "start",apks->pks[i].stages[j].start );
			js_write_int(&jswork, "num",apks->pks[i].stages[j].num );
			js_write_int(&jswork, "tf",apks->pks[i].stages[j].tf );
			js_write_int(&jswork, "stop",apks->pks[i].stages[j].stop );
			js_write_bool(&jswork, "plus",apks->pks[i].stages[j].plus );
			js_write_bool(&jswork, "trs",apks->pks[i].stages[j].trs );
			js_write_int(&jswork, "dt",apks->pks[i].stages[j].dt );
			js_write_value_end(&jswork);
		}
		js_write_array_end(&jswork);
		js_write_value_end(&jswork);
	}
	js_write_array_end(&jswork);
	js_write_end(&jswork);
	return jswork.start;
}
char* OnePKToJsonString(AllPks *apks,int pk,char* buffer,size_t size) {
	js_write jswork;
	if (buffer==NULL){
		js_write_start(&jswork, size);
	} else {
		js_write_static(&jswork,buffer,size);
	}
	for (int i = 0; i < MAX_PKS ; ++i) {
		if(apks->pks[i].pk!=pk) continue;
		js_write_int(&jswork, "pk",apks->pks[i].pk );
		js_write_int(&jswork, "tpu",apks->pks[i].tpu);
		js_write_bool(&jswork, "razlen",apks->pks[i].razlen);
		js_write_int(&jswork, "tc",apks->pks[i].tc);
		js_write_int(&jswork, "shift",apks->pks[i].shift);
		js_write_bool(&jswork, "twot",apks->pks[i].twot);
		js_write_array_start(&jswork, "sts");
		for (int j = 0; j < MAX_STAGES; ++j) {
			js_write_value_start(&jswork, "");
			js_write_int(&jswork, "line",apks->pks[i].stages[j].line );
			js_write_int(&jswork, "start",apks->pks[i].stages[j].start );
			js_write_int(&jswork, "num",apks->pks[i].stages[j].num );
			js_write_int(&jswork, "tf",apks->pks[i].stages[j].tf );
			js_write_int(&jswork, "stop",apks->pks[i].stages[j].stop );
			js_write_bool(&jswork, "plus",apks->pks[i].stages[j].plus );
			js_write_bool(&jswork, "trs",apks->pks[i].stages[j].trs );
			js_write_int(&jswork, "dt",apks->pks[i].stages[j].dt );
			js_write_value_end(&jswork);
		}
		js_write_array_end(&jswork);
	}
	js_write_end(&jswork);
	return jswork.start;
}
bool OnePKFromJsonString(char *root, AllPks *apks,int pk) {
	js_read jswork;
	js_read jsstage;
	js_read jsline;

	js_read_start(&jswork, root);
	for (int i = 0; i < MAX_PKS; ++i) {
		if (apks->pks[i].pk!=pk) continue;
		js_read_int(&jswork, "pk",&apks->pks[i].pk );
		js_read_int(&jswork, "tpu",&apks->pks[i].tpu );
		js_read_bool(&jswork, "razlen",&apks->pks[i].razlen );
		js_read_int(&jswork, "tc",&apks->pks[i].tc );
		js_read_int(&jswork, "shift",&apks->pks[i].shift );
		js_read_bool(&jswork, "twot",&apks->pks[i].twot );
		if(js_read_array(&jswork, &jsstage, "sts")!=JsonSuccess) return false;
		for (int j = 0; j < MAX_STAGES; ++j) {
			if(js_read_array_object(&jsstage, j, &jsline)!=JsonSuccess) return false;
			js_read_int(&jsline, "line",&apks->pks[i].stages[j].line);
			js_read_int(&jsline, "start",&apks->pks[i].stages[j].start);
			js_read_int(&jsline, "num",&apks->pks[i].stages[j].num);
			js_read_int(&jsline, "tf",&apks->pks[i].stages[j].tf);
			js_read_int(&jsline, "stop",&apks->pks[i].stages[j].stop);
			js_read_bool(&jsline, "plus",&apks->pks[i].stages[j].plus);
			js_read_bool(&jsline, "trs",&apks->pks[i].stages[j].trs);
			js_read_int(&jsline, "dt",&apks->pks[i].stages[j].dt);
		}
		return true;
	}
	return false;
}


void AllPksFromJsonString(char *root, AllPks *apks) {
	js_read jswork;
	js_read jsdk;
	js_read jspk;
	js_read jsstage;
	js_read jsline;

	js_read_start(&jswork, root);
	js_read_array(&jswork, &jsdk, "dk");
	for (int i = 0; i < MAX_PKS; ++i) {
		if (js_read_array_object(&jsdk, i, &jspk)!=JsonSuccess) break;
		js_read_int(&jspk, "pk",&apks->pks[i].pk );
		js_read_int(&jspk, "tpu",&apks->pks[i].tpu );
		js_read_bool(&jspk, "razlen",&apks->pks[i].razlen );
		js_read_int(&jspk, "tc",&apks->pks[i].tc );
		js_read_int(&jspk, "shift",&apks->pks[i].shift );
		js_read_bool(&jspk, "twot",&apks->pks[i].twot );
		js_read_array(&jspk, &jsstage, "sts");
		for (int j = 0; j < MAX_STAGES; ++j) {
			if(js_read_array_object(&jsstage, j, &jsline)!=JsonSuccess) break;
			js_read_int(&jsline, "line",&apks->pks[i].stages[j].line);
			js_read_int(&jsline, "start",&apks->pks[i].stages[j].start);
			js_read_int(&jsline, "num",&apks->pks[i].stages[j].num);
			js_read_int(&jsline, "tf",&apks->pks[i].stages[j].tf);
			js_read_int(&jsline, "stop",&apks->pks[i].stages[j].stop);
			js_read_bool(&jsline, "plus",&apks->pks[i].stages[j].plus);
			js_read_bool(&jsline, "trs",&apks->pks[i].stages[j].trs);
			js_read_int(&jsline, "dt",&apks->pks[i].stages[j].dt);
		}
	}
}


