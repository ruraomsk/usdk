/*
 * FazeWork.c
 *
 *  Created on: 13 сент. 2021 г.
 *      Author: rura
 */

#include <stdlib.h>
#include "CommonData.h"
#include "core_json.h"
void clearPhasesSet(PhasesSet *phasesSet) {
	DefinePhase empty = { .num = 0, .work = false };
	for (int i = 0; i < MAX_PHASES; ++i) {
		empty.num = i;
		phasesSet->defPhase[i] = empty;
	}
}
char* PhasesSetToJsonString(PhasesSet *phasesSet,char* buffer,size_t size) {
	js_write jswork;
	if (buffer==NULL){
		js_write_start(&jswork, size);
	} else {
		js_write_static(&jswork,buffer,size);
	}
	js_write_array_start(&jswork, "phases");
	for (int i = 0; i < MAX_PHASES ; ++i) {
		js_write_value_start(&jswork, "");
		js_write_bool(&jswork, "work", phasesSet->defPhase[i].work);
		js_write_int(&jswork, "num",phasesSet->defPhase[i].num );
		js_write_value_end(&jswork);
	}
	js_write_array_end(&jswork);
	js_write_end(&jswork);
	return jswork.start;
}
char* OnePhaseToJsonString(PhasesSet *phasesSet,int phase,char* buffer,size_t size) {
	js_write jswork;
	if (buffer==NULL){
		js_write_start(&jswork, size);
	} else {
		js_write_static(&jswork,buffer,size);
	}
	for (int i = 0; i < MAX_PHASES ; ++i) {
		if (phase!=phasesSet->defPhase[i].num) continue;
		js_write_int(&jswork, "num",phasesSet->defPhase[i].num );
		js_write_bool(&jswork, "work", phasesSet->defPhase[i].work);
	}
	js_write_end(&jswork);
	return jswork.start;
}
bool OnePhaseFromJsonString(char *root, PhasesSet *phasesSet,int phase) {
	js_read jswork;
	js_read_start(&jswork, root);
	for (int i = 0; i < MAX_PHASES; ++i) {
		if(phasesSet->defPhase[i].num!=phase) continue;
		js_read_bool(&jswork, "work", &phasesSet->defPhase[i].work);
		js_read_int(&jswork, "num",&phasesSet->defPhase[i].num );
		return true;
	}
	return false;
}
void PhasesSetFromJsonString(char *root, PhasesSet *phasesSet) {
	js_read jswork;
	js_read jsarray;
	js_read object;
	js_read_start(&jswork, root);
	if(js_read_array(&jswork, &jsarray, "phases")!=JsonSuccess) return;
	for (int i = 0; i < MAX_PHASES; ++i) {
		if (js_read_array_object(&jsarray, i, &object)!=JsonSuccess) break;
		js_read_bool(&object, "work", &phasesSet->defPhase[i].work);
		js_read_int(&object, "num",&phasesSet->defPhase[i].num );
	}
}

DefinePhase* getPhase(PhasesSet *phasesSet, int num) {
	for (int i = 0; i < MAX_PHASES; ++i) {
		if (phasesSet->defPhase[i].num == num) {
			return &phasesSet->defPhase[i];
		}
	}
	return NULL;
}

