/*
 * share.c
 *
 *  Created on: Aug 11, 2021
 *      Author: rura
 */

#include "Files.h"
#include "ff.h"
#include <search.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "DebugLogger.h"
osMutexId_t ShareMutex;


osMutexId_t FilesMutex;
void LockFiles(void) {
	osStatus_t result = osMutexAcquire(FilesMutex, osWaitForever);
	if (result != osOK && result != osErrorISR) {
		Debug_Message(LOG_ERROR, "Не смог заблокировать файловую систему");
	}
}
void UnlockFiles(void) {
	osMutexRelease(FilesMutex);
}
FATFS fs;
FIL fil;
UINT bw;
FRESULT result;
BYTE work[FF_MAX_SS];
static int ReadyFS=0;
int FilesInit() {
	//Проверим создана ли уже файловая система
	if(ReadyFS) return 0;
	FilesMutex = osMutexNew(NULL);
	Debug_Message(LOG_INFO, "Создаем файловую систему");
	result = f_mkfs("", 0, work, sizeof work);
	if (result) {
		Debug_Message(LOG_ERROR, "Ошибка создания файловой системы %d", result);
		return 1;
	}
	result = f_mount(&fs, "", 0);
	if (result) {
		Debug_Message(LOG_ERROR, "Ошибка монтирования файловой системы %d", result);
		return 1;
	}
	//Это пока ФС на RAM
	f_mkdir("set");
	Debug_Message(LOG_INFO, "Файловая система готова");
	ReadyFS=1;
	return 0;
}

char* FilesGetJsonString(char *filename) {
	if (osMutexAcquire(FilesMutex, osWaitForever) == osOK) {
		result = f_open(&fil, filename, FA_READ);
		if (result) {
			Debug_Message(LOG_ERROR, "Ошибка %d открытия файла %s", result, filename);
			osMutexRelease(FilesMutex);
			return NULL;
		}
		size_t size = f_size(&fil);
		char *buffer = pvPortMalloc(size);
		if (buffer == NULL) {
			Debug_Message(LOG_ERROR, "Нет памяти  %d для чтения файла %s", size, filename);
			osMutexRelease(FilesMutex);
			return NULL;
		}
		f_read(&fil, buffer, size, &bw);
		f_close(&fil);
		osMutexRelease(FilesMutex);
		return buffer;
	}
	return NULL;
}
void FilesSetJsonString(char *filename, char* value) {
	if (osMutexAcquire(FilesMutex, osWaitForever) == osOK) {
		result = f_open(&fil, filename, FA_CREATE_NEW | FA_WRITE);
		if (result) {
			Debug_Message(LOG_ERROR, "Ошибка %d открытия файла %s", result, filename);
			osMutexRelease(FilesMutex);
			return;
		}
		f_write(&fil, value, strlen(value), &bw);
		if (bw !=strlen(value) ) {
			Debug_Message(LOG_ERROR, "Ошибка записи в файл %s %d байт", filename, bw);
		}
		f_close(&fil);
		osMutexRelease(FilesMutex);
	}
}
