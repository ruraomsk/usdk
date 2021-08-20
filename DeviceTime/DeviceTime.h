/*
 * DeviceTime.h
 *
 *  Created on: Aug 19, 2021
 *      Author: rura
 */

#ifndef DEVICETIME_H_
#define DEVICETIME_H_

#define DeviceTimeStep 100
typedef unsigned long int dev_time;

void DeviceTimeInit();
dev_time GetDeviceTime(void);
char* TimeToString(dev_time time);
void UpdateDeviceTime(void);


#endif /* DEVICETIME_H_ */