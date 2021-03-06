/*
 * YearSet.c
 *
 *  Created on: 14 сент. 2021 г.
 *      Author: rura
 */

#include <stdlib.h>
#include "core_json.h"
#include "CommonData.h"
void clearYearSet(YearSet *yearSet) {
	OneMonth empty;
	for (int i = 0; i < 31; ++i) {
		empty.weeks[i] = 1;
	}
	for (int i = 0; i < 12; ++i) {
		empty.num = i + 1;
		yearSet->months[i] = empty;
	}
}
int getYearDay(YearSet *yearSet, int month, int day) {
	if (month < 1 || month > 12) return 0;
	if (day < 1 || day > 31) return 0;
	for (int i = 0; i < 12; ++i) {
		if (yearSet->months[i].num == month) {
			return yearSet->months[i].weeks[day - 1];
		}
	}
	return 0;
}
char* YearSetToJsonString(YearSet *yearSet,char* buffer,size_t size) {
	js_write jswork;
	if (buffer==NULL){
		js_write_start(&jswork, size);
	} else {
		js_write_static(&jswork,buffer,size);
	}
	js_write_array_start(&jswork, "monthset");
	for (int i = 0; i < 12 ; ++i) {
		js_write_value_start(&jswork, "");
		js_write_byte(&jswork, "num", yearSet->months[i].num);
		js_write_array_start(&jswork, "days");
		for (int j = 0; j < 31; ++j) {
			js_write_byte(&jswork,"",yearSet->months[i].weeks[j]);
		}
		js_write_array_end(&jswork);
		js_write_value_end(&jswork);
	}
	js_write_array_end(&jswork);
	js_write_end(&jswork);
	return jswork.start;
}
char* OneMonthToJsonString(YearSet *yearSet,int month,char* buffer,size_t size){
	js_write jswork;
	if (buffer==NULL){
		js_write_start(&jswork, size);
	} else {
		js_write_static(&jswork,buffer,size);
	}
	for (int i = 0; i < 12 ; ++i) {
		if(month!=yearSet->months[i].num) continue;
		js_write_byte(&jswork, "num", yearSet->months[i].num);
		js_write_array_start(&jswork, "days");
		for (int j = 0; j < 31; ++j) {
			js_write_byte(&jswork,"",yearSet->months[i].weeks[j]);
		}
		js_write_array_end(&jswork);
		break;
	}
	js_write_end(&jswork);
	return jswork.start;
}
bool OneMonthFromJsonString(char* root, YearSet *yearSet,int month){
	js_read jswork,jslines;
	js_read_start(&jswork, root);
	for (int i = 0; i < 12; ++i) {
		if(yearSet->months[i].num!=month) continue;
		js_read_byte(&jswork, "num",&yearSet->months[i].num );
		if(js_read_array(&jswork, &jslines, "days")!=JsonSuccess) return false;
		int j=0;
		while(js_read_array_byte(&jslines, j, &yearSet->months[i].weeks[j])==JsonSuccess){
			j++;
			if(j>30) break;
		}
		return true;
	}
	return false;
}
void YearSetFromJsonString(char* root, YearSet *yearSet) {
	js_read jswork;
	js_read jsdays;
	js_read jsmonth;
	js_read jslines;
	js_read_start(&jswork, root);
	if(js_read_array(&jswork, &jsdays, "monthsets")!=JsonSuccess) return;
	for (int i = 0; i < 12; ++i) {
		if (js_read_array_object(&jsdays, i, &jsmonth)!=JsonSuccess) break;
		js_read_byte(&jsmonth, "num",&yearSet->months[i].num );
		js_read_array(&jsmonth, &jslines, "days");
		for (int j = 0; j < 31; ++j) {
			if (js_read_array_byte(&jslines, j, &yearSet->months[i].weeks[j])!=JsonSuccess) break;
		}
	}

}

