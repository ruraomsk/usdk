/*
 * WeekSet.c
 *
 *  Created on: 14 сент. 2021 г.
 *      Author: rura
 */

#include <stdlib.h>
#include "CommonData.h"
void clearWeekSet(WeekSet *weekSet) {
	OneWeek empty;
	for (int i = 0; i < 7; ++i) {
		empty.days[i] = 1;
	}
	for (int i = 0; i < MAX_WEEKS; ++i) {
		empty.num = i+1;
		weekSet->weeks[i] = empty;
	}
}
int getWeekDay(WeekSet *weekSet, int week, int day) {
	if (day < 1 || day > 7) return 0;
	for (int i = 0; i < MAX_WEEKS; ++i) {
		if (weekSet->weeks[i].num == week) {
			return weekSet->weeks[i].days[day - 1];
		}
	}
	return 0;
}
char* WeekSetToJsonString(WeekSet *weekSet) {
	js_write jswork;
	js_write_start(&jswork, 2400);
	js_write_array_start(&jswork, "wsets");
	for (int i = 0; i < MAX_WEEKS ; ++i) {
		js_write_value_start(&jswork, "");
		js_write_int(&jswork, "num",weekSet->weeks[i].num );
		js_write_array_start(&jswork, "days");
		for (int j = 0; j < 7; ++j) {
			js_write_int(&jswork,"",weekSet->weeks[i].days[j]);
		}
		js_write_array_end(&jswork);
		js_write_value_end(&jswork);
	}
	js_write_array_end(&jswork);
	return jswork->start;
}
void WeekSetFromJsonString(char* root, WeekSet *weekSet) {
	js_read jswork;
	js_read jswsets;
	js_read jsdays;
	js_read objweek;
	js_read_start(&jswork, root);
	js_read_array(&jswork, &jswsets, "wsets");
	for (int i = 0; i < MAX_WEEKS ; ++i) {
		if (js_read_array_object(&jswsets, i, &objweek)!=JsonSuccess) break;
		js_read_int(&objweek, "num",&weekSet->weeks[i].num );
		js_read_array(&objweek, &jsdays, "days");
		for (int j = 0; j < 7; ++j) {
			if (js_read_array_int(&jsdays,j,&weekSet->weeks[i].days[j])!=JsonSuccess) break;
		}
	}
}
