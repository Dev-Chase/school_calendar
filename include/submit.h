#ifndef CALENDAR_SUBMIT_H
#define CALENDAR_SUBMIT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "calendar.h"

void submit_work(const char *cal_path, const char *opt, DF works[static 1]);

#ifdef __cplusplus
}
#endif

#endif
