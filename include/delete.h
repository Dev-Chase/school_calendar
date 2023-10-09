#ifndef CALENDAR_DELETE_H
#define CALENDAR_DELETE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "calendar.h"

void delete_work(const char *cal_path, DF works[static 1], const char *opt);

#ifdef __cplusplus
}
#endif

#endif
