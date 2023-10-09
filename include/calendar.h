#ifndef CALENDAR_H
#define CALENDAR_H

#ifdef __cplusplus
extern "C" {
#endif

#define _XOPEN_SOURCE /* See feature_test_macros(7) */
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>

typedef struct DataFrame {
  uint_fast32_t len_rows;
  FILE *fp;
} DataFrame;
typedef DataFrame DF;

#define WORKS_NAME "/works.csv"
#define N_COLS 4

#define MAX_COURSE_LEN 52   // 51 + \0
#define MAX_NAME_LEN 175    // 174 + \0
#define MAX_DATE_LEN 20     // strlen("September 30, 2023") +\n + \0
#define MAX_TIME_LEN 7      // strlen("23:59") + \n + \0
#define MAX_SUBMITTED_LEN 6 // strlen("False") + \0

#define CONFIRMATION_LEN 3   // 'y' + \n + \0
#define MAX_FILE_DATE_LEN 26 // strlen("10 September 2008 - 02:00") + \0
#define MAX_ROW_LEN                                                            \
  (MAX_COURSE_LEN + MAX_NAME_LEN + MAX_FILE_DATE_LEN + MAX_SUBMITTED_LEN + 5)

#define COURSE_COL 0
#define NAME_COL 1
#define DUE_COL 2
#define SUB_COL 3

#define SEP_CH '|'

void create_scal_path(char *dest, const char *cal_path, const char *fname);
bool cmp_field(const char *field, const char *cmp_field);
uint_fast32_t select_row(DF works[static 1], const char *opt);
void open_data_frame(char *cal_path, DF *works);
void print_field(const char *field);
bool cmp_time(char *row, const char *cmp_time);
int8_t get_date(char out[static MAX_FILE_DATE_LEN], struct tm *date_out);

#ifdef __cplusplus
}
#endif

#endif
