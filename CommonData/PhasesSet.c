/*
 * FazeWork.c
 *
 *  Created on: 13 сент. 2021 г.
 *      Author: rura
 */

#include <stdlib.h>
#include "CommonData.h"
#include "core_json.h"
void setPhase(PhasesSet *phasesSet,DefinePhase ph) {
	for (int i = 0; i < MAX_PHASES; ++i) {
		if (phasesSet->defPhase[i].num == ph.num) {
			phasesSet->defPhase[i]=ph;
			return;
		}
	}
}

void clearPhasesSet(PhasesSet *phasesSet) {
	DefinePhase empty = { .num = 0, .work = false };
	for (int i = 0; i < MAX_PHASES; ++i) {
		empty.num = i+1;
		phasesSet->defPhase[i] = empty;
	}
	//@formater:off
	DefinePhase fillPhases[] ={
			{ .num = 1,.Tmin=10,.Tprom=3, .work = true },
			{ .num = 2,.Tmin=14,.Tprom=4, .work = true },
			{ .num = 3,.Tmin=15,.Tprom=5, .work = true },
			{ .num = YellowBlink,.Tmin=10,.Tprom=5, .work = true },
			{ .num = OffLight,.Tmin=5,.Tprom=0, .work = true },
			{ .num = AllRed,.Tmin=5,.Tprom=5, .work = true },
			{ .num = -1},

	};
	//@formater:on
	for (int i = 0; fillPhases[i].num!=-1; ++i) {
		setPhase(phasesSet, fillPhases[i]);
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
		js_write_int(&jswork, "tmin",phasesSet->defPhase[i].Tmin );
		js_write_int(&jswork, "tprom",phasesSet->defPhase[i].Tprom );
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
		js_write_int(&jswork, "tmin",phasesSet->defPhase[i].Tmin );
		js_write_int(&jswork, "tprom",phasesSet->defPhase[i].Tprom );
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
		js_read_int(&jswork, "tmin",&phasesSet->defPhase[i].Tmin );
		js_read_int(&jswork, "tprom",&phasesSet->defPhase[i].Tprom );
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
		js_read_int(&object, "tmin",&phasesSet->defPhase[i].Tmin );
		js_read_int(&object, "tprom",&phasesSet->defPhase[i].Tprom );
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

