/*
 * support.c
 *
 *  Created on: 28 окт. 2021 г.
 *      Author: rura
 */
#include "CommonData.h"
#include "Technology.h"

bool isWorkPhase(int phase){
	switch (phase) {
		case YellowBlink:
		case OffLight:
		case AllRed:
			return false;
		default:
			return true;
	}
}


