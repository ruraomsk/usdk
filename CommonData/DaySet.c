/*
 * DaySet.c
 *
 *  Created on: 14 сент. 2021 г.
 *      Author: rura
 */
#include <stdlib.h>
#include "core_json.h"
#include "CommonData.h"

void clearDaySet(DaySet *daySet) {
	OneDay empty;
	Line line={.npk=0,.time=0};
	for (int i = 0; i < MAX_DAYS; ++i) {
		empty.count=0;
		empty.num=i+1;
		for (int j = 0; j < MAX_LINES; ++j) {
			empty.lines[j]=line;
		}
		daySet->days[i] = empty;
	}
}
OneDay* getOneDay(DaySet *daySet, int num) {
	for (int i = 0; i < MAX_DAYS; ++i) {
		if (daySet->days[i].num == num) {
			return &daySet->days[i];
		}
	}
	return NULL;
}
char* DaySetToJsonString(DaySet *daySet,size_t size) {
	js_write jswork;
	js_write_start(&jswork, size);
	js_write_array_start(&jswork, "daysets");
	for (int i = 0; i < MAX_DAYS ; ++i) {
		js_write_value_start(&jswork, "");
		js_write_int(&jswork, "num",daySet->days[i].num );
		js_write_int(&jswork, "count",daySet->days[i].count );
		js_write_array_start(&jswork, "lines");
		for (int j = 0; j < MAX_LINES; ++j) {
			js_write_value_start(&jswork, "");
			js_write_int(&jswork, "npk",daySet->days[i].lines[j].npk );
			js_write_int(&jswork, "hour",daySet->days[i].lines[j].time/60 );
			js_write_int(&jswork, "min",daySet->days[i].lines[j].time%60 );
			js_write_value_end(&jswork);
		}
		js_write_array_end(&jswork);
		js_write_value_end(&jswork);
	}
	js_write_array_end(&jswork);
	js_write_end(&jswork);
	return jswork.start;
}
void DaySetFromJsonString(char *root, DaySet *daySet) {
	js_read jswork;
	js_read jsdays;
	js_read jslines;
	js_read objday;
	js_read objline;
	js_read_start(&jswork, root);
	js_read_array(&jswork, &jsdays, "daysets");
	for (int i = 0; i < MAX_DAYS; ++i) {
		if (js_read_array_object(&jsdays, i, &objday)!=JsonSuccess) break;
		js_read_int(&objday, "num",&daySet->days[i].num );
		js_read_int(&objday, "count",&daySet->days[i].count );
		js_read_array(&objday, &jslines, "lines");
		for (int j = 0; j < MAX_LINES; ++j) {
			if(js_read_array_object(&jslines, j, &objline)!=JsonSuccess) break;
			int min,hour;
			js_read_int(&objline, "npk",&daySet->days[i].lines[j].npk );
			js_read_int(&objline, "hour",&hour );
			js_read_int(&objline, "min",&min );
			daySet->days[i].lines[j].time=(hour*60)+min;
		}
	}
}

