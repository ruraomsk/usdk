/*
 * Camera.h
 *
 *  Created on: Sep 2, 2021
 *      Author: rura
 */

#ifndef CAMERA_H_
#define CAMERA_H_

#include "DeviceTime.h"
#include "CommonData.h"

#define CAMERA_READ_ZONE 	1
#define CAMERA_STATISTICS 	2
#define CAMERA_STEP_CONTROL 100U
#define CAMERA_MAX_ZONE 	4

// Занятость по зонам
typedef struct{
	int id;
	time_t time;
	int zones[CAMERA_MAX_ZONE];
}OneCamOcup;
typedef struct{
	OneCamOcup cameras[MAX_CAMERAS];
}CamerasOcup;

typedef struct{
	int zone;
	int intensiv;
	int speed;
	int density;
	int cameras;
	int gp;
}OneZona;
typedef struct{
	int id;
	time_t time;
	OneZona zones[CAMERA_MAX_ZONE];
}OneCameraStat;
typedef struct {
	OneCameraStat cameras[MAX_CAMERAS];
}CameraStat;

void CameraWork(void *arg);


void clearZoneOcup(CamerasOcup *zo);
void appendOneZone(char* buffer,CamerasOcup *zo,int i,int id);

#endif /* CAMERA_H_ */
