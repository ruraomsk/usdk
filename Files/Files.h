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

int FilesInit(void);


void ShareSaveChange(void);
JSON_Value* FilesGetJson(char *chart);
void FilesSetJson(char *chart, JSON_Value *value);
void LockFiles(void);
void UnlockFiles(void);

#ifdef __cplusplus
}
#endif

#endif /* SHARE_H_ */
