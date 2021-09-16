/*
 * DeviceLogger.c
 *
 *  Created on: 18 авг. 2021 г.
 *      Author: rura
 */

#include "DeviceLogger.h"
#include "DeviceTime.h"
#include "DebugLogger.h"
#include "ff.h"
#include "Files.h"

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

static SubNames DevLogWork[] = { { SUB_TRANSPORT, "Транспорт", NULL }, { SUB_FILES, "Файл", NULL }, { SUB_END, "\0",
		NULL } };
static osMutexId_t LogDevMutex;
static RingBuffer *devicelogs;
DeviceLoggerMessage msg, oldmsg;
void DeviceLogInit() {
	LogDevMutex = osMutexNew(NULL);
	devicelogs = newRingBuffer(CAPACITY_MESSAGES, sizeof(DeviceLoggerMessage));
	if (devicelogs == NULL) {
		Debug_Message(LOG_FATAL, "Невозможно создать лог устройства");
		return;
	}
	SubNames *result = DevLogWork;
	while (result->subsystem != SUB_END) {
		result->lastMessage = pvPortMalloc(MAX_LEN_MESSAGE);
		memset(result->lastMessage, 0, MAX_LEN_MESSAGE);
		result++;
	}
	Debug_Message(LOG_INFO, "Создан лог устройства");
}
SubNames* getSubsystem(char sub) {
	SubNames *result = DevLogWork;
	while (result->subsystem != SUB_END) {
		if (result->subsystem == sub)
			return result;
		result++;
	}
	return NULL;
}
FRESULT result;
FIL file;
void saveRingBufferToFile() {
	LockFiles();
	FATFS *fs;
	DWORD free_clust, free_sect;
	if (f_getfree("", &free_clust, &fs)) {
		Debug_Message(LOG_ERROR, "Ошибка чтения свободного места для  файла сохранения логов");
		while (RingBufferTryRead(devicelogs, (void*) &oldmsg) == RINGBUFFER_OK) {
		}
		UnlockFiles();
		return;
	}
	free_sect = free_clust * fs->csize;
	if (free_sect < NEED_FOR_SAVING) {
		Debug_Message(LOG_INFO, "Нет свободного места для  файла сохранения логов удаляем его...");
		f_unlink("logdev");
	}
	if (f_open(&file, "logdev", FA_WRITE | FA_OPEN_ALWAYS) != FR_OK) {
		Debug_Message(LOG_ERROR, "ошибка открытия файла сохранения логов");
		UnlockFiles();
		return;
	}
	if (f_lseek(&file, f_size(&file)) != FR_OK) {
		Debug_Message(LOG_ERROR, "ошибка перемещения в конец файла сохранения логов");
		f_close(&file);
		UnlockFiles();
		return;
	}
	char *buf = pvPortMalloc(MAX_LEN_MESSAGE);
	if (buf!=NULL){
		while (RingBufferTryRead(devicelogs, (void*) &oldmsg) == RINGBUFFER_OK) {
			UINT bw;
			sprintf(buf, "%s\t%s\t%s\n", TimeToString(oldmsg.time), getSubsystem(oldmsg.subsystem)->name, oldmsg.message);
			f_write(&file, buf, strlen(buf), &bw);
		}
	}
	vPortFree(buf);
	f_close(&file);
	UnlockFiles();
}
void DeviceLog(char subsytem, char *fmt, ...) {
	HeapStats_t pxHeapStats;

	vPortGetHeapStats(&pxHeapStats);

	if (osMutexAcquire(LogDevMutex, osWaitForever) == osOK) {
		SubNames *subName = getSubsystem(subsytem);
		if (subName != NULL) {
			va_list ap;
			va_start(ap, fmt);
			char *message = pvPortMalloc(3 * MAX_LEN_MESSAGE);
			if(message==NULL) return;
			vsprintf(message, fmt, ap);
			message[MAX_LEN_MESSAGE - 1] = 0;
			//Если такое сообщение уже посылали то и нефиг его записывать
			if (strcmp(subName->lastMessage, message) != 0) {
				strcpy(subName->lastMessage, message);
				msg.time = GetDeviceTime();
				msg.subsystem = subsytem;
				strcpy(msg.message, message);
				while (RingBufferTryWrite(devicelogs, (void*) &msg) != RINGBUFFER_OK) {
					Debug_Message(LOG_INFO, "Лог устройства полон");
					saveRingBufferToFile();
					Debug_Message(LOG_INFO, "Лог устройства сохранен в файл");
				}
			}
			vPortFree(message);
		} else {
			Debug_Message(LOG_ERROR, "Неверный номер подсистемы");
		}
		osMutexRelease(LogDevMutex);
	}
	vPortGetHeapStats(&pxHeapStats);
}

JSON_Value* DeviceLogToJSON() {
	JSON_Value *result = NULL;
	return result;
}

