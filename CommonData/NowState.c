/*
 * NowState.c
 *
 *  Created on: 7 окт. 2021 г.
 *      Author: rura
 */
#include <stdlib.h>
#include <string.h>
#include "core_json.h"
#include "CommonData.h"
#include "CommonExt.h"

void clearNowState(NowState* ns) {
	memset(ns,0,sizeof(NowState));
}
char* NowStateToJsonString(NowState* ns,char* buffer,size_t size) {
	// @formatter:off
	js_write w;
	if (buffer==NULL){
		js_write_start(&w, size);
	} else {
		js_write_static(&w,buffer,size);
	}
	js_write_int(&w, "tech", ns->tech);
	js_write_bool(&w, "base", ns->base);
	js_write_int(&w, "pk", ns->pk);
	js_write_int(&w, "ck", ns->ck);
	js_write_int(&w, "nk", ns->nk);
	js_write_int(&w, "tmax", ns->tmax);

	js_write_value_start(&w, "comdu");
		js_write_bool(&w,"IsPK",ns->comdu.IsPK);
		js_write_bool(&w,"IsCK",ns->comdu.IsCK);
		js_write_bool(&w,"IsNK",ns->comdu.IsNK);
		js_write_bool(&w,"IsDUDK1",ns->comdu.IsDUDK1);
		js_write_bool(&w,"IsDUDK2",ns->comdu.IsDUDK2);
		js_write_bool(&w,"IsReqSFDK1",ns->comdu.IsReqSFDK1);
		js_write_bool(&w,"IsReqSFDK2",ns->comdu.IsReqSFDK2);
		js_write_int(&w, "pk", ns->comdu.pk);
		js_write_int(&w, "ck", ns->comdu.ck);
		js_write_int(&w, "nk", ns->comdu.nk);
		js_write_int(&w, "phase", ns->comdu.phase);

	js_write_value_end(&w);
	js_write_array_start(&w, "dks");
	for (int i = 0; i < 2; ++i) {
		js_write_value_start(&w, "");
			js_write_int(&w,"rdk",ns->dk[i].fdk);
			js_write_int(&w,"fdk",ns->dk[i].fdk);
			js_write_int(&w,"ddk",ns->dk[i].ddk);
			js_write_int(&w,"edk",ns->dk[i].edk);
			js_write_bool(&w,"pdk",ns->dk[i].pdk);
			js_write_int(&w,"eedk",ns->dk[i].eedk);
			js_write_bool(&w,"odk",ns->dk[i].odk);
			js_write_int(&w,"ldk",ns->dk[i].ldk);
			js_write_int(&w,"ftudk",ns->dk[i].ftudk);
			js_write_int(&w,"tdk",ns->dk[i].tdk);
			js_write_int(&w,"ftsdk",ns->dk[i].ftsdk);
			js_write_int(&w,"ttcdk",ns->dk[i].ttcdk);
			js_write_int(&w,"exts",ns->dk[i].exts);
		js_write_value_end(&w);
	}
	js_write_array_end(&w);
	js_write_end(&w);
	return w.start;
	// @formatter:on
}
void NowStateFromJsonString(char *root, NowState* ns) {
	// @formatter:off
	js_read w;
	js_read_start(&w, root);
	js_read_int(&w, "tech", &ns->tech);
	js_read_bool(&w, "base", &ns->base);
	js_read_int(&w, "pk", &ns->pk);
	js_read_int(&w, "ck", &ns->ck);
	js_read_int(&w, "nk", &ns->nk);
	js_read_int(&w, "tmax", &ns->tmax);
	js_read c;
	js_read_value(&w, "comdu",&c);
		js_read_bool(&c,"IsPK",&ns->comdu.IsPK);
		js_read_bool(&c,"IsCK",&ns->comdu.IsCK);
		js_read_bool(&c,"IsNK",&ns->comdu.IsNK);
		js_read_bool(&c,"IsDUDK1",&ns->comdu.IsDUDK1);
		js_read_bool(&c,"IsDUDK2",&ns->comdu.IsDUDK2);
		js_read_bool(&c,"IsReqSFDK1",&ns->comdu.IsReqSFDK1);
		js_read_bool(&c,"IsReqSFDK2",&ns->comdu.IsReqSFDK2);
		js_read_int(&c,"pk",&ns->comdu.pk);
		js_read_int(&c,"ck",&ns->comdu.ck);
		js_read_int(&c,"nk",&ns->comdu.nk);
		js_read_int(&c,"phase",&ns->comdu.phase);
	js_read d,dks;
	if(js_read_array(&w, &dks, "dks")!=JsonSuccess) return;
	for (int i = 0; i < 2; ++i) {
		if(js_read_array_object(&dks,i,&d)!=JsonSuccess) return;
			js_read_int(&d,"rdk",&ns->dk[i].fdk);
			js_read_int(&d,"fdk",&ns->dk[i].fdk);
			js_read_int(&d,"ddk",&ns->dk[i].ddk);
			js_read_int(&d,"edk",&ns->dk[i].edk);
			js_read_bool(&d,"pdk",&ns->dk[i].pdk);
			js_read_int(&d,"eedk",&ns->dk[i].eedk);
			js_read_bool(&d,"odk",&ns->dk[i].odk);
			js_read_int(&d,"ldk",&ns->dk[i].ldk);
			js_read_int(&d,"ftudk",&ns->dk[i].ftudk);
			js_read_int(&d,"tdk",&ns->dk[i].tdk);
			js_read_int(&d,"ftsdk",&ns->dk[i].ftsdk);
			js_read_int(&d,"ttcdk",&ns->dk[i].ttcdk);
			js_read_int(&d,"exts",&ns->dk[i].exts);
	}
	// @formatter:on
}



