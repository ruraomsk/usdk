/*
 * ControlData.c
 *
 *  Created on: Oct 15, 2021
 *      Author: rura
 */

#include "CommonData.h"
#include "Common_private.h"
#include "DebugLogger.h"
#include "Files.h"
#include <string.h>
#include "Transport.h"
#include "service.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdarg.h>

void appendMessage(char *buffer, char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	int len = strlen(buffer);
	vsnprintf(buffer + len, MAX_LEN_TCP_MESSAGE - 1 - len, fmt, ap);
}
bool weeks [ MAX_WEEKS ];
bool days [ MAX_DAYS ];
bool apks [ MAX_PKS ];
bool phs [MAX_PHASES];
bool ControlData(char *buffer, YearSet *ys, WeekSet *ws, DaySet *ds, AllPks *pks, PhasesSet *ps) {
	*buffer = 0;
	memset(weeks, 0, sizeof(weeks));
	memset(days, 0, sizeof(days));
	memset(apks, 0, sizeof(pks));
	memset(phs, 0, sizeof(phs));
	for (int m = 0; m < 12; ++m) {
		if (ys->months [ m ].num != m + 1) {
			appendMessage(buffer, "[нет %d месяца в ГК]", m + 1);
			break;
		}
		//Месячный план
		for (int w = 0; w < 31; ++w) {
			//Выбрали недельный план для дня месяца проверим что он есть
			// он должен быть и быть не пустым
			uint8_t wp = ys->months [ m ].weeks [ w ];
			if (wp < 1 || wp > MAX_WEEKS) {
				appendMessage(buffer, "[ошибка НК %d %d дня в %d месяце в ГК]", wp, w + 1, m + 1);
				break;
			}
			//Теперь найдем такой недельный план
			if (!weeks [ wp - 1 ]) {
				//Недельную карту еще не проверяли
				weeks [ wp - 1 ] = true;
				if (ws->weeks [ wp - 1 ].num != wp) {
					appendMessage(buffer, "[нет НК %d %d дня в %d месяце в ГК]", wp, w + 1, m + 1);
					break;
				}
				for (int i = 0; i < 7; ++i) {
					uint8_t ck=ws->weeks [ wp - 1 ].days[i];
					if(ck<1||ck>MAX_DAYS) {
						appendMessage(buffer, "[ошибка СК %d в НК %d ]",ck, wp);
						break;
					}
					if(!days[ck-1]) {
						days[ck-1]=true;
						if(ds->days[ck-1].num!=ck){
							appendMessage(buffer, "[нет СК %d в НК %d ]",ck, wp);
							break;
						}
						if (ds->days[ck-1].count==0){
							appendMessage(buffer, "[пустая СК %d в НК %d ]",ck, wp);
							break;
						}
						for (int j = 0; j < ds->days[ck-1].count; ++j) {
							int pk=ds->days[ck-1].lines[j].npk;
							if(pk<1||pk>MAX_PKS){
								appendMessage(buffer, "[ошибка ПК %d в СК %d ]",pk,ck);
								break;
							}
							if(!apks[pk-1]){
								apks[pk-1]=true;
								if(pk!=pks->pks[pk-1].pk){
									appendMessage(buffer, "[нет ПК %d в СК %d ]",pk,ck);
									break;
								}
								for (int s = 0; s < MAX_STAGES; ++s) {
									int ph=pks->pks[pk-1].stages[s].num;
									if(ph<0||ph>=MAX_PHASES){
										appendMessage(buffer, "[Ошибка фазы %d в ПК %d]",ph,pk);
										break;
									}
									if(!phs[ph]){
										phs[ph]=true;
										if(ps->defPhase[ph].num!=ph){
											appendMessage(buffer, "[Ошибка фазы %d]",ph);
											break;
										}
										if(!ps->defPhase[ph].work){
											appendMessage(buffer, "[Фаза %d отключена]",ph);
											break;
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}

	return strlen(buffer) == 0 ? true : false;
}

