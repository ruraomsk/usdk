/*
 * ToFiles.c
 *
 *  Created on: Sep 1, 2021
 *      Author: rura
 */
#include "ff.h"
#include "DebugLogger.h"
void initFiles() {
	FATFS fs;
	FIL fil;
	UINT bw;
	FRESULT res;
	BYTE work[FF_MAX_SS];
	Debug_Message(LOG_INFO, "Создаем файловую систему");
	res = f_mkfs("", 0, work, sizeof work);
	if (res ) {
		Debug_Message(LOG_ERROR, "Ошибка создания %d", res);
		return;
	}
	res = f_mount(&fs, "", 0);
	if (res ) {
		Debug_Message(LOG_ERROR, "Ошибка монтирования %d", res);
		return;
	}
	res = f_open(&fil, "hello.txt", FA_CREATE_NEW | FA_WRITE);
	if (res ) {
		Debug_Message(LOG_ERROR, "Ошибка открытия файла %d", res);
		return;
	}
	f_write(&fil, "Hello, World!\r\n", 15, &bw);
	if (bw != 15) {
		Debug_Message(LOG_ERROR, "Ошибка записи в файл  %d байт", bw);
	}
	/* Close the file */
	f_close(&fil);
	f_mount(0, "", 0);

}

