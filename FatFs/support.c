/*
 * support.c
 *
 *  Created on: Sep 1, 2021
 *      Author: rura
 */
#include "diskio.h"
#include <string.h>
static void *RAM_BUFFER = NULL;
#define RAM_SIZE_SECTOR 512
#define RAM_COUNT_SECTORS 700

int RAM_VALID(LBA_t sector, UINT count) {
	if (count < 1)
		return RES_ERROR;
	if ((sector + count) > RAM_COUNT_SECTORS)
		return RES_ERROR;
	return RES_OK;
}
int RAM_disk_status() {
	if (RAM_BUFFER != NULL)
		return RES_OK;
	return RES_ERROR;
}
int MMC_disk_status() {
	return RES_ERROR;
}
int USB_disk_status() {
	return RES_ERROR;
}
int RAM_disk_initialize() {
	if (RAM_BUFFER != NULL)
		return RES_OK;
	RAM_BUFFER = malloc(RAM_SIZE_SECTOR * RAM_COUNT_SECTORS);
	if (RAM_BUFFER != NULL)
		return RES_OK;
	return RES_ERROR;
}
int MMC_disk_initialize() {
	return RES_ERROR;
}
int USB_disk_initialize() {
	return RES_ERROR;
}
int RAM_disk_read(BYTE *buff, LBA_t sector, UINT count) {
	if (RAM_VALID(sector, count) != RES_OK)
		return RES_ERROR;
	memcpy(buff, RAM_BUFFER + sector * RAM_SIZE_SECTOR,
			RAM_SIZE_SECTOR * count);
	return RES_OK;
}
int MMC_disk_read(BYTE *buff, LBA_t sector, UINT count) {
	return RES_ERROR;
}
int USB_disk_read(BYTE *buff, LBA_t sector, UINT count) {
	return RES_ERROR;
}
int RAM_disk_write(const BYTE *buff, LBA_t sector, UINT count) {
	if (RAM_VALID(sector, count) != RES_OK)
		return RES_ERROR;
	memcpy(RAM_BUFFER + sector * RAM_SIZE_SECTOR, buff,
			RAM_SIZE_SECTOR * count);
	return RES_OK;
}
int MMC_disk_write(const BYTE *buff, LBA_t sector, UINT count) {
	return RES_ERROR;
}
int USB_disk_write(const BYTE *buff, LBA_t sector, UINT count) {
	return RES_ERROR;
}
int RAM_disk_ioctl(BYTE pdrv, /* Physical drive nmuber (0..) */
BYTE cmd, /* Control code */
void *buff /* Buffer to send/receive control data */
) {
	int *rez = buff;
	switch (cmd) {
	case CTRL_SYNC:
		return RES_OK;
	case GET_SECTOR_COUNT:
		*rez = RAM_COUNT_SECTORS;
		return RES_OK;
	case GET_SECTOR_SIZE:
		*rez = RAM_SIZE_SECTOR;
		return RES_OK;
	}
	return RES_ERROR;
}
int MMC_disk_ioctl(BYTE pdrv,	BYTE cmd,void *buff) {
	return RES_ERROR;
}
int USB_disk_ioctl(BYTE pdrv,	BYTE cmd,void *buff) {
	return RES_ERROR;
}

