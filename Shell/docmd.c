/*
 * docmd.c
 *
 *  Created on: 12 нояб. 2021 г.
 *      Author: rura
 */
#include "Shell.h"
#include <stdbool.h>
#include <string.h>
#include "ff.h"

typedef struct {
	char* name;
	int code;
	bool needParam;
} oneCmd;
typedef struct {
	int code;
	char* param;
} parsCmd;
// @formatter:off
oneCmd listCmd[]={	{.name="cwd",.code=1,.needParam=false},  			//cwd 	текущий каталог
					{.name="ls",.code=2,.needParam=false},				//ls  	список файлов в директории
					{.name="cd",.code=3,.needParam=true},
					{.name="get",.code=4,.needParam=true},


					{NULL,0,false},				//Конец списка команд
};
char dir[80];
// @formatter:on
void cmdInit(){
	strcpy(dir,"/");
}
void sayReady(char* buffer){
	uint8_t endline [ ] = "\r\n";
	buffer[0]=0;
	strcpy(buffer,(char*)endline);
	strcat(buffer,dir);
	strcat(buffer,">");
	CDC_Transmit_FS((uint8_t*)buffer, strlen(buffer));
}
parsCmd parserCmd(char* buffer){
	parsCmd result={.code=0,.param=NULL};
	for (int i = 0; listCmd[i].name != NULL ; ++i) {
		if(	memcmp(buffer,listCmd[i].name,strlen(listCmd[i].name))!=0) continue;
		result.code=listCmd[i].code;
		if(!listCmd[i].needParam) break;
		result.param=buffer+strlen(listCmd[i].name)+1;
		while (*result.param==' ')result.param++;
		break;
	}
	return result;
}
void doCmd(char* buffer){
	parsCmd cmd=parserCmd(buffer);
	switch (cmd.code) {
		case 1:
			strcpy(buffer,dir);
			return;
		case 2:
			dircommand(dir,buffer);
			return;
		case 3:
			cdcommand(dir,cmd.param,buffer);
			return;
		case 4:
			sendFile(dir,cmd.param,buffer);
			return;
		default:
			strcat(buffer," command not found");
			return;
	}

}
