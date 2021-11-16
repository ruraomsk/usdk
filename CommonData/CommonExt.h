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
	int rdk;		//Режим ДК
	// 1 2 Ручное управление
	// 3 Зеленая улица
	// 4 Диспетчерское управление
	// 5 6 Локальное управление
	// 8 9 Координированное управление
	int fdk;	//Фаза ДК
	// от 1 до 8 номера рабочих фаз
	// 9 промежуточный такт
	// 10 желтое мигание
	// 11 отключен светофор
	// 12 кругом краснный
	int ddk;	//Устройство ДК
	//	1 - ДК
	//	2 - ВПУ
	//	3 - инженерный пульт (ИП УСДК)
	//	4 - УСДК/ДКА
	//	5 - инженерная панель (ИП ДКА)
	//	6 - система (ЭВМ)
	//	7 - система (ЭВМ)
	// 	8 - система (ЭВМ)
	// 	9 - система (ЭВМ)
	int edk;	//Неисправность ДК
	bool pdk;	//Признак переходного периода ДК
	int eedk;	//дополнительный код неисправности
	bool odk;	//Открыта дверь ДК
	int ldk;	//Номер фазы на которой сгорели лампы
	int ftudk;	//Фаза ТУ ДК на момент передачи
	int tdk;	//Время отработки ТУ в секундах
	int ftsdk;	//Фаза ТС ДК
	int ttcdk;	//Время от начала фазы ТС в секундах
	int exts;	//Расширенный статус устройства
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
