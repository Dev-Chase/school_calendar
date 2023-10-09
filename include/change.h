#ifndef CALENDAR_DATE_H
#define CALENDAR_DATE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "calendar.h"

void change_work_date(const char *cal_path, DF works[static 1],
					  const char *opt);
void change_work_name(const char *cal_path, DF works[static 1],
					  const char *opt);

#ifdef __cplusplus
}
#endif

#endif
