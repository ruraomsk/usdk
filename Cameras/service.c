/*
 * service.c
 *
 *  Created on: Oct 6, 2021
 *      Author: rura
 */
#include "Camera.h"
#include <string.h>
#include <stdio.h>
void clearZoneOcup(CamerasOcup *zo){
	memset(zo,0,sizeof(CamerasOcup));
}
void appendOneZone(char* buffer,CamerasOcup *zo,int i,int id){
	zo->cameras[i].id=id;
	zo->cameras[i].time=GetDeviceTime();
	char delim[]=" \n\r";
	char* p=strtok(buffer+3,delim);
	while (p!=NULL){
		int zone,value;
		sscanf(p,"%d",&zone);
		p=strtok(NULL,delim);
		sscanf(p,"%d",&value);
		p=strtok(NULL,delim);
		if (zone<1 || zone>CAMERA_MAX_ZONE) break;
		zo->cameras[i].zones[zone-1]=value;
	}
}
