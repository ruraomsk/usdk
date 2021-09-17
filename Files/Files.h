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

int FilesInit(void);
char* FilesGetJsonString(char *path);
void FilesSetJsonString(char *path, char *value);
void LockFiles(void);
void UnlockFiles(void);

#ifdef __cplusplus
}
#endif

#endif /* SHARE_H_ */
