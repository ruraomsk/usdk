/*
 * share.c
 *
 *  Created on: Aug 11, 2021
 *      Author: rura
 */

#include "Files.h"
#include "ff.h"
#include "parson.h"
#include <search.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "DebugLogger.h"
char *charts[] = { "debug", "cmain","csec", "setup", "camera","\0" };

char *values[] = {
		"{\"ip\":\"192.168.115.159\",\"port\":2095}",
		"{\"ip\":\"192.168.115.159\",\"port\":2093,\"timeoutread\":30,\"timeoutwrite\":10,\"timeoutque\":1}",
		"{\"ip\":\"192.168.115.159\",\"port\":2094,\"timeoutread\":10,\"timeoutwrite\":10,\"timeoutque\":30}",
		"{\"id\":\128,\"eth\":true,\"gprs\":true,\"gps\":true,\"usb\":true,\"modbusslave\":true,\"modbusmaster\":true}",
		"{\"ip\":\"192.168.115.168\",\"port\":8441,\"login\":\"admin\",\"password\":\"admin\",\"step\":10}",
		"{\"ip\":\"192.168.115.168\"}",
		NULL
		 };
osMutexId_t ShareMutex;

extern int ReadyFiles;

char *filename;
osMutexId_t FilesMutex;

unsigned int max_size=0;
FATFS fs;
FIL fil;
UINT bw;
FRESULT res;
BYTE work[FF_MAX_SS];
int FilesInit() {
	FilesMutex = osMutexNew(NULL);
	//Проверим создана ли уже файловая система
	if (ReadyFiles) return 0;
	filename=malloc(120);
	Debug_Message(LOG_INFO, "Создаем файловую систему");
	res = f_mkfs("", 0, work, sizeof work);
	if (res ) {
		Debug_Message(LOG_ERROR, "Ошибка создания файловой системы %d", res);
		return 1;
	}
	res = f_mount(&fs, "", 0);
	if (res ) {
		Debug_Message(LOG_ERROR, "Ошибка монтирования файловой системы %d", res);
		return 1;
	}
	for (int i = 0; ; ++i) {
		if (strlen(charts[i])==0) break;
		sprintf(filename,"%s",charts[i]);
		res = f_open(&fil,filename, FA_CREATE_NEW | FA_WRITE);
		if (res ) {
			Debug_Message(LOG_ERROR, "Ошибка %d открытия файла %s", res,filename);
			break ;
		}
		f_write(&fil, values[i], strlen(values[i]), &bw);
		if (bw != strlen(values[i])) {
			Debug_Message(LOG_ERROR, "Ошибка записи в файл %s %d байт",filename, bw);
		} else {
			max_size=bw>max_size?bw:max_size;
		}
		f_close(&fil);
	}
	ReadyFiles = 1;
	Debug_Message(LOG_INFO, "Файловая система готова");
	return 0;
}


JSON_Value* ShareGetJson(char *chart) {
	JSON_Value* result=NULL;
	if (!ReadyFiles) return result;
	if (osMutexAcquire(FilesMutex, osWaitForever) == osOK) {
		sprintf(filename,"%s",chart);
		res = f_open(&fil,filename, FA_READ);
		if (res ) {
			Debug_Message(LOG_ERROR, "Ошибка %d открытия файла %s", res,filename);
			osMutexRelease(FilesMutex);
			return result;
		}
		char *buffer=malloc(max_size);
		if (buffer==NULL){
			Debug_Message(LOG_ERROR, "Нет памяти  %d для чтения файла %s", max_size,filename);
			osMutexRelease(FilesMutex);
			return result;
		}
		f_read(&fil, buffer, max_size, &bw);
		result=json_parse_string(buffer);
		f_close(&fil);
		free(buffer);
		osMutexRelease(FilesMutex);
		return result;
	}
	return result;
}
void ShareSetJson(char *chart, JSON_Value *rvalue) {
	if (!ReadyFiles) return;
	if (osMutexAcquire(FilesMutex, osWaitForever) == osOK) {
		sprintf(filename,"%s",chart);
		size_t size=json_serialization_size(rvalue);
		char *buffer=malloc(size);
		if (buffer==NULL){
			Debug_Message(LOG_ERROR, "Нет памяти  %d для записи файла %s", max_size,filename);
			osMutexRelease(FilesMutex);
			return;
		}
		json_serialize_to_buffer(rvalue, buffer, size);
		res = f_open(&fil,filename, FA_CREATE_NEW | FA_WRITE);
		if (res ) {
			Debug_Message(LOG_ERROR, "Ошибка %d открытия файла %s", res,filename);
			osMutexRelease(FilesMutex);
			return;
		}
		f_write(&fil, buffer, size, &bw);
		if (bw != size) {
			Debug_Message(LOG_ERROR, "Ошибка записи в файл %s %d байт",filename, bw);
		} else {
			max_size=size>max_size?size:max_size;
		}
		f_close(&fil);
		osMutexRelease(FilesMutex);
	}
}
