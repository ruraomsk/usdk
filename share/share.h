/*
 * share.h
 * Управление общей пямятью системы в том числе настройками
 *  Created on: Aug 11, 2021
 *      Author: rura
 */

#ifndef SHARE_H_
#define SHARE_H_
#ifdef __cplusplus
extern "C" {
#endif

#include <search.h>
#include "cmsis_os.h"
#include "parson.h"

void ShareInit(void);

typedef struct sharevalue {
	JSON_Value *json_root;
	int changed;
} ShareValue;

void ShareSaveChange(void);
JSON_Value* ShareGetJson(const char *chart);
void ShareSetJson(const char *chart, JSON_Value *value);

#ifdef __cplusplus
}
#endif

#endif /* SHARE_H_ */
