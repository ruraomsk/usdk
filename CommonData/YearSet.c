/*
 * YearSet.c
 *
 *  Created on: 14 сент. 2021 г.
 *      Author: rura
 */

#include <stdlib.h>
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
char* YearSetToJsonString(YearSet *yearSet) {
	js_write jswork;
	js_write_start(&jswork, 2400);
	js_write_array_start(&jswork, "monthset");
	for (int i = 0; i < 12 ; ++i) {
		js_write_value_start(&jswork, "");
		js_write_int(&jswork, "num", yearSet->months[i].num);
		js_write_array_start(&jswork, "days");
		for (int j = 0; j < 31; ++j) {
			js_write_int(&jswork,"",yearSet->months[i].days[j]);
		}
		js_write_array_end(&jswork);
		js_write_value_end(&jswork);
	}
	js_write_array_end(&jswork);
	return jswork->start;
}
void YearSetFromJsonString(char* root, YearSet *yearSet) {
}

