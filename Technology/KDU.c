/*
 * DK.c
 *
 *  Created on: 19 окт. 2021 г.
 *      Author: rura
 */
#include "CommonData.h"
#include "Technology.h"
/*
 * Имитирует исполнение команд обычного ДК
 * Имеет на борту локальный план и настройки Тмин и Тпром
 */

// @formatter:off
//SetPk lp={.stages={
//							{.line=1,.start=0,.stop=30,.num=1},
//							{.line=2,.start=30,.stop=60,.num=2},
//							{.line=3,.start=60,.stop=90,.num=3},
//							{.line=-1}}};
// @formatter:on
void KDUWork(void *arg) {
	SetupKDU *skdu = (SetupKDU*) arg;
	/*
	 *	Имеем три очереди
	 *	1 - очередь команд от контроллера
	 *	2 - очередь внутренняя  команд для выбора 3 из трех
	 *	3 - выходная очередь состояния устройства в нее помещаем состояние автомата каждую секунду
	 *	Краткий алгоритм
	 *	каждую секунду проверяем наличие команд. Отсутствие команд означает потерю управления над нами
	 *	Команды выбираются по принципу три совпадения подряд.
	 *	После трех совпадений команда помещается в очередь ожидающих задач
	 *	Проверяем состояние автомата готовы ли мы принять команду управления то есть закончился промтакт и выработано Тмин для
	 *	данной фазы.
	 *	Если да то меняем состояние автомата на новую команду
	 *
	 */
	OneStep statusKDU;
	int retPhase;
	int incmd [ 3 ] = { LocalPlane, LocalPlane, LocalPlane };
	int nowCmd;
	/*
	 * Локальные функции для реентерабельности
	 */
	bool setStatus(int phase, int len) {
		for (int i = 0; i < MAX_PHASES; ++i) {
			if (phase != skdu->phs.defPhase[ i ].num) continue;
			if (!skdu->phs.defPhase[ i ].work) return false;
			statusKDU.TTmin = skdu->phs.defPhase[ i ].Tmin;
			statusKDU.TTprom = skdu->phs.defPhase [ i ].Tprom;
			statusKDU.phase = phase;
			statusKDU.TLen = len < 0 ? INT64_MAX : len;
			if (statusKDU.TTmin > statusKDU.TLen) statusKDU.TLen = statusKDU.TTmin;
			return true;
		}
		return false;
	}
	void startLP() {
		setStatus(skdu->lp.stages [ 0 ].num, skdu->lp.stages [ 0 ].stop - skdu->lp.stages [ 0 ].start);
		statusKDU.pos = 0;
		nowCmd = LocalPlane;
	}
	void stepLP() {
		if (statusKDU.TLen) return;
		statusKDU.pos++;
		if (statusKDU.pos >= MAX_STAGES || skdu->lp.stages [ statusKDU.pos ].num == 0) {
			startLP();
			return;
		}
		setStatus(skdu->lp.stages [ statusKDU.pos ].num,
				skdu->lp.stages [ statusKDU.pos ].stop - skdu->lp.stages [ statusKDU.pos ].start);
	}
	int readInQueCMD() {
		if (osMessageQueueGetCount(skdu->inCommand) != 0) {
			int cmd;
			if (osMessageQueueGet(skdu->inCommand, &cmd, 0, osWaitForever) == osOK) return cmd;
		}
		return LocalPlane;
	}
	void sendPhase() {
		osMessageQueuePut(skdu->outPhase, &retPhase, 0, osWaitForever);
	}
	void addCmd(int cmd) {
		incmd [ 0 ] = incmd [ 1 ];
		incmd [ 1 ] = incmd [ 2 ];
		incmd [ 2 ] = cmd;
	}
	void stepStatus() {
		if (statusKDU.TTprom) statusKDU.TTprom--;
		if (statusKDU.TTmin) statusKDU.TTmin--;
		if (statusKDU.TLen) statusKDU.TLen--;
		if (statusKDU.TTprom)
			retPhase = PromTakt;
		else
			retPhase = statusKDU.phase;
	}
	/*
	 * Стартовая точка задачи
	 */

	setStatus(AllRed, -1);
	nowCmd = AllRed;
	for (;;) {
		osDelay(1000U);
		stepStatus();
		if (nowCmd == LocalPlane) stepLP();
		addCmd(readInQueCMD());
		if (incmd [ 0 ] == incmd [ 1 ] && incmd [ 1 ] == incmd [ 2 ]) {
			int cmd = incmd [ 0 ];
			if (cmd != nowCmd && statusKDU.TTmin == 0) {
				switch (cmd) {
				case LocalPlane:
					startLP();
					break;
				case YellowBlink:
					setStatus(YellowBlink, -1);
					nowCmd = cmd;
					break;
				case OffLight:
					setStatus(OffLight, -1);
					nowCmd = cmd;
				case AllRed:
					setStatus(AllRed, -1);
					nowCmd = cmd;
					break;
				default:
					if (setStatus(cmd, -1)) nowCmd = cmd;
					break;
				}
			}
		}
		sendPhase();
	}
}
