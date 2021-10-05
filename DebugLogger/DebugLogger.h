#ifndef __LOGGER_H
#define __LOGGER_H

#include "cmsis_os.h"
#include "DeviceTime.h"
//#include "lwip.h"
//#include "lwip/udp.h"


#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	int Level;
	time_t time;
	size_t size;
	char Buffer[128];
} DebugLoggerMsg;
void Debug_Init(void);
void Debug_Message(int level,char *fmt,...);
char *Debuger_Status(int level);

void DebugLoggerLoop(void);

void CrashMemory();
#define LOG_FATAL 0
#define LOG_ERROR 1
#define LOG_DEBUG 2
#define LOG_INFO  9


#ifdef __cplusplus
}
#endif

#endif /* __LOGGER_H */
