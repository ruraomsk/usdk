/*
 * CommonExt.h
 *
 *  Created on: 7 окт. 2021 г.
 *      Author: rura
 */

#ifndef COMMONEXT_H_
#define COMMONEXT_H_

#include <stdint.h>
#include <stdbool.h>

//Состояние GPS
typedef struct{
	bool Ok;		//	Все исправно
	bool E01;		// 	Нет связи с приемником
	bool E02;		//	Ошибка CRC
	bool E03;		//  Нет валидного времени
	bool E04;		//  Мало спутников
	bool Seek;		//  Поиск спутников после включения
} GPSSet;
#define GPSSetName "gps"
typedef struct {
	bool IsPK;
	bool IsCK;
	bool IsNK;
	bool IsDUDK1;
	bool IsDUDK2;
	bool IsReqSFDK1;
	bool IsReqSFDK2;
	int pk;				//Назначенный ПК
	int ck;				//Назначенный СК
	int nk;				//Назначенный НК
	int phase;			//Назначенная фаза
}CommandDU;
typedef struct {
	int rdk;
	int fdk;
	int ddk;
	int edk;
	bool pdk;
	int eedk;
	bool odk;
	int ldk;
	int ftudk;
	int tdk;
	int ftsdk;
	int ttcdk;
}DK;
typedef struct{
	int tech;
	bool base;
	int pk;
	int ck;
	int nk;
	int tmax;
	CommandDU comdu;
	DK dk[2];
}NowState;
#define NowStateName "nst"



#endif /* COMMONEXT_H_ */
