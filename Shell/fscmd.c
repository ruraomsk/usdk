/*
 * fscmd.c
 *
 *  Created on: 15 нояб. 2021 г.
 *      Author: rura
 */

#include "ff.h"
#include "Shell.h"
#include <string.h>
#include <stdio.h>

char fullpath[120];
extern uint8_t UserTxBufferFS [ APP_TX_DATA_SIZE ];
void makeFullPath(char* path,char* filename){
	if (filename[0]=='/') {
		strcpy(fullpath,filename);
	} else {
		strcpy(fullpath,path);
		if (path[strlen(path)-1]!='/') strcat(fullpath,"/");
		strcat(fullpath,filename);
	}

}
void dircommand(char* path,char* buffer){
	DIR dp;
	FILINFO fno;
	FRESULT res=f_opendir(&dp, path);
	char buf[80];
	buffer[0]=0;
	if (res!=FR_OK){
		strcpy(buffer,"not open dir");
		return;
	}
	while (f_readdir(&dp, &fno)==FR_OK){
		if(strlen(fno.fname)==0) break;
		strcat(buffer,fno.fname);
		strcat(buffer,";");
		if ((fno.fattrib&AM_DIR)!=0){
			strcat(buffer,"subdir;");
		} else {
			strcat(buffer,"file;");
		}
		sprintf(buf,"%ld$",fno.fsize);
		strcat(buffer,buf);
	}
	f_closedir(&dp);
}
void cdcommand(char* path,char* newpath,char* buffer){
	buffer[0]=0;
	if (newpath[0]=='/') {
		strcpy(path,newpath);
	} else {
		if (path[strlen(path)-1]!='/') strcat(path,"/");
		strcat(path,newpath);
	}
	strcpy(buffer,path);
}
void sendFile(char* path,char* filename,char* buffer){
	FIL fp;
	UINT br;
	buffer[0]=0;
	makeFullPath(path, filename);
	FRESULT r=f_open(&fp, fullpath,FA_READ );
	if (r!=FR_OK){
		strcpy(buffer,"not open file");
		strcat(buffer,fullpath);
		return;
	}
	FSIZE_t size=f_size(&fp);
	while(size>0){
		FSIZE_t len=(size>APP_TX_DATA_SIZE)?APP_TX_DATA_SIZE:size;
		r=f_read(&fp, UserTxBufferFS,len, &br);
		if (r!=FR_OK){
			strcpy(buffer,"dont read file");
			strcat(buffer,fullpath);
			f_close(&fp);
			return;
		}
		writeDataToUsb(UserTxBufferFS, len);
		size-=len;
	}
	f_close(&fp);
	strcpy(buffer,"ok");
}

