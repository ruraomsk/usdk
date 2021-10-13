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


void LockFiles(void) {
}
void UnlockFiles(void) {
}
FIL fil;
UINT bw;
FRESULT result;
FATFS fs;
BYTE work[FF_MAX_SS];

static int ReadyFS=0;
int FilesInit() {
	//Проверим создана ли уже файловая система
	if(ReadyFS) return 0;
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
		result = f_open(&fil, filename, FA_READ);
		if (result) {
			Debug_Message(LOG_ERROR, "Ошибка %d открытия файла %s", result, filename);
			return NULL;
		}
		size_t size = f_size(&fil);
		char *buffer = pvPortMalloc(size);
		if (buffer == NULL) {
			Debug_Message(LOG_ERROR, "Нет памяти  %d для чтения файла %s", size, filename);
			return NULL;
		}
		f_read(&fil, buffer, size, &bw);
		if (bw !=size ) {
			Debug_Message(LOG_ERROR, "Ошибка чтения файл %s %d байт", filename, bw);
		}
		f_close(&fil);
		return buffer;
}
void FilesSetJsonString(char *filename, char* value) {
		result = f_open(&fil, filename, FA_CREATE_NEW | FA_WRITE);
		if (result) {
			Debug_Message(LOG_ERROR, "Ошибка %d открытия файла %s", result, filename);
			return;
		}
		f_write(&fil, value, strlen(value), &bw);
		if (bw !=strlen(value) ) {
			Debug_Message(LOG_ERROR, "Ошибка записи в файл %s %d байт", filename, bw);
		}
		f_close(&fil);
}
