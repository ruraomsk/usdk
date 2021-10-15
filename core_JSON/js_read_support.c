/*
 * js_read_support.c
 *
 *  Created on: Sep 16, 2021
 *      Author: rura
 */
#include "cmsis_os.h"
#include "core_json.h"
#include <strings.h>
#include <string.h>
#include "cmsis_os.h"
#include <stdarg.h>
#include <stdio.h>
extern bool arraySearch( const char * buf,size_t max,size_t queryIndex,size_t * outValue,size_t * outValueLength );

JSONStatus_t js_read_array(js_read* w, js_read *array,char* name){
	JSONStatus_t result = JSON_Search( w->buffer, w->size, name, strlen(name),&w->value, &w->valueLength);
	if (result != JsonSuccess) return result;
	array->buffer=w->value;
	array->size=w->valueLength;
	return JsonSuccess;
}
JSONStatus_t js_read_array_object(js_read* array,size_t index,js_read* object){
	size_t val;
	JSONStatus_t result = arraySearch( array->buffer, array->size, index,&val, &array->valueLength);
	if (result != JsonSuccess) return result;
	object->buffer=array->buffer+val;
	object->size=array->valueLength;
	return JsonSuccess;
}
JSONStatus_t js_read_array_array(js_read* array,size_t index,js_read* newarray){
	size_t val;
	JSONStatus_t result = arraySearch( array->buffer, array->size, index,&val, &array->valueLength);
	if (result != JsonSuccess) return result;
	newarray->buffer=array->buffer+val;
	newarray->size=array->valueLength;
	return JsonSuccess;
}

JSONStatus_t js_read_array_int(js_read* array,size_t index,int *value){
	size_t val;
	JSONStatus_t result = arraySearch( array->buffer, array->size, index,&val, &array->valueLength);
	if (result != JsonSuccess) return result;
	char save=array->buffer[val+array->valueLength];
	array->buffer[val+array->valueLength]=0;
	sscanf(array->buffer+val,"%i",value);
	array->buffer[val+array->valueLength]=save;
	return JsonSuccess;
}
JSONStatus_t js_read_array_byte(js_read* array,size_t index,uint8_t *value){
	size_t val;
	int int_value;
	JSONStatus_t result = arraySearch( array->buffer, array->size, index,&val, &array->valueLength);
	if (result != JsonSuccess) return result;
	char save=array->buffer[val+array->valueLength];
	array->buffer[val+array->valueLength]=0;
	sscanf(array->buffer+val,"%i",&int_value);
	*value=int_value&0xff;
	array->buffer[val+array->valueLength]=save;
	return JsonSuccess;
}
JSONStatus_t js_read_array_string(js_read* array,size_t index,char *value){
	size_t val;
	JSONStatus_t result = arraySearch( array->buffer, array->size, index,&val, &array->valueLength);
	if (result != JsonSuccess) return result;
	char save=array->buffer[val+array->valueLength];
	array->buffer[val+array->valueLength]=0;
	sscanf(array->buffer+val,"%s",value);
	array->buffer[val+array->valueLength]=save;
	return JsonSuccess;
}
JSONStatus_t js_read_array_double(js_read* array,size_t index,double *value){
	size_t val;
	JSONStatus_t result = arraySearch( array->buffer, array->size, index,&val, &array->valueLength);
	if (result != JsonSuccess) return result;
	char save=array->buffer[val+array->valueLength];
	array->buffer[val+array->valueLength]=0;
	sscanf(array->buffer+val,"%lf",value);
	array->buffer[val+array->valueLength]=save;
	return JsonSuccess;
}
JSONStatus_t js_read_array_bool(js_read* array,size_t index,bool *value){
	size_t val;
	JSONStatus_t result = arraySearch( array->buffer, array->size, index,&val, &array->valueLength);
	if (result != JsonSuccess) return result;
	if (*(array->buffer+val)==0x54 ||*(array->buffer+val)==0x74 )	*value=true;
	else *value=false;
	return JsonSuccess;
}

JSONStatus_t js_read_start(js_read* w, char* buffer){
	w->buffer=buffer;
	w->size=strlen(buffer);
	return JSON_Validate( w->buffer, w->size);
}
JSONStatus_t js_read_int(js_read* w,char* name,int *value){
	JSONStatus_t result = JSON_Search( w->buffer, w->size, name, strlen(name),&w->value, &w->valueLength);
	if (result != JsonSuccess) return result;
	char save=w->value[w->valueLength];
	w->value[w->valueLength]=0;
	sscanf(w->value,"%i",value);
	w->value[w->valueLength]=save;
	return JsonSuccess;
}
JSONStatus_t js_read_byte(js_read* w,char* name,uint8_t *value){
	int int_value;
	JSONStatus_t result = JSON_Search( w->buffer, w->size, name, strlen(name),&w->value, &w->valueLength);
	if (result != JsonSuccess) return result;
	char save=w->value[w->valueLength];
	w->value[w->valueLength]=0;
	sscanf(w->value,"%i",&int_value);
	*value=int_value&0xff;
	w->value[w->valueLength]=save;
	return JsonSuccess;
}
JSONStatus_t js_read_string(js_read* w,char* name,char *value){
	JSONStatus_t result = JSON_Search( w->buffer, w->size, name, strlen(name),&w->value, &w->valueLength);
	if (result != JsonSuccess) return result;
	char save=w->value[w->valueLength];
	w->value[w->valueLength]=0;
	sscanf(w->value,"%s",value);
	w->value[w->valueLength]=save;
	return JsonSuccess;
}
JSONStatus_t js_read_double(js_read* w,char* name,double *value){
	JSONStatus_t result = JSON_Search( w->buffer, w->size, name, strlen(name),&w->value, &w->valueLength);
	if (result != JsonSuccess) return result;
	char save=w->value[w->valueLength];
	w->value[w->valueLength]=0;
	sscanf(w->value,"%lf",value);
	w->value[w->valueLength]=save;
	return JsonSuccess;
}
JSONStatus_t js_read_bool(js_read* w,char* name,bool *value){
	JSONStatus_t result = JSON_Search( w->buffer, w->size, name, strlen(name),&w->value, &w->valueLength);
	if (result != JsonSuccess) return result;
	if (*w->value==0x54 ||*w->value==0x74 )	*value=true;
	else *value=false;
	return JsonSuccess;
}
JSONStatus_t js_read_value(js_read* w,char* name,js_read* value){
	JSONStatus_t result = JSON_Search( w->buffer, w->size, name, strlen(name),&w->value, &w->valueLength);
	if (result != JsonSuccess) return result;
	value->buffer=w->value;
	value->size=w->valueLength;
	return JsonSuccess;
}


