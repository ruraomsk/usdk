/*
 * serial_JSON.c
 *
 *  Created on: Sep 15, 2021
 *      Author: rura
 */
#include "cmsis_os.h"
#include "core_json.h"
#include <strings.h>
#include <string.h>
#include "cmsis_os.h"
#include <stdarg.h>
#include <stdio.h>

JSONStatus_t js_write_start(js_write *work, size_t size) {
	work->start = pvPortMalloc(size + 12);
	if (work->start == NULL) return JSONNotMemory;
	work->pos = work->start;
	work->size = size;
	*work->pos++ = '{';
	return JsonSuccess;
}
JSONStatus_t js_write_end(js_write *w) {
	w->pos--;
	snprintf(w->pos, w->size - (w->pos - w->start), "}");
	w->pos += strlen(w->start);
	return JsonSuccess;
}
void js_write_free(js_write *work) {
	vPortFree(work->start);
}
JSONStatus_t js_write_int(js_write *w, char *name, int value) {
	if (strlen(name) == 0)
		snprintf(w->pos, w->size - (w->pos - w->start), "%d,", value);
	else
		snprintf(w->pos, w->size - (w->pos - w->start), "\"%s\":%d,", name, value);
	w->pos = w->start + strlen(w->start);
	return JsonSuccess;
}
JSONStatus_t js_write_string(js_write *w, char *name, char *value) {
	if (strlen(name) == 0)
		snprintf(w->pos, w->size - (w->pos - w->start), "\"%s\",", value);
	else
		snprintf(w->pos, w->size - (w->pos - w->start), "\"%s\":\"%s\",", name, value);
	w->pos = w->start + strlen(w->start);
	return JsonSuccess;
}
JSONStatus_t js_write_bool(js_write *w, char *name, bool value) {
	if (strlen(name) == 0) {
		if (value)
			snprintf(w->pos, w->size - (w->pos - w->start), "true,");
		else
			snprintf(w->pos, w->size - (w->pos - w->start), "false,");

	} else {
		if (value)
			snprintf(w->pos, w->size - (w->pos - w->start), "\"%s\"true,", name);
		else
			snprintf(w->pos, w->size - (w->pos - w->start), "\"%s\"false,", name);
	}
	w->pos = w->start + strlen(w->start);
	return JsonSuccess;
}
JSONStatus_t js_write_double(js_write *w, char *name, double value) {
	if (strlen(name) == 0)
		snprintf(w->pos, w->size - (w->pos - w->start), "%f,", value);
	else
		snprintf(w->pos, w->size - (w->pos - w->start), "\"%s\":%f,", name, value);
	w->pos = w->start + strlen(w->start);
	return JsonSuccess;
}
JSONStatus_t js_write_value_start(js_write *w, char *name) {
	if (strlen(name) == 0)
		snprintf(w->pos, w->size - (w->pos - w->start), "{");
	else
		snprintf(w->pos, w->size - (w->pos - w->start), "\"%s\":{", name);
	w->pos = w->start + strlen(w->start);
	return JsonSuccess;
}
JSONStatus_t js_write_value_end(js_write *w) {
	w->pos--;
	snprintf(w->pos, w->size - (w->pos - w->start), "},");
	w->pos = w->start + strlen(w->start);
	return JsonSuccess;
}
JSONStatus_t js_write_array_start(js_write *w, char *name) {
	if (strlen(name) == 0)
		snprintf(w->pos, w->size - (w->pos - w->start), "[");
	else
		snprintf(w->pos, w->size - (w->pos - w->start), "\"%s\":[", name);
	w->pos = w->start + strlen(w->start);
	return JsonSuccess;
}
JSONStatus_t js_write_array_end(js_write *w) {
	w->pos--;
	snprintf(w->pos, w->size - (w->pos - w->start), "],");
	w->pos = w->start + strlen(w->start);
	return JsonSuccess;
}

