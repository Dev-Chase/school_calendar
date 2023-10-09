#include "add.h"
#include <ctype.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

void add_work(DF *works) {
  puts("=========================================================");
  puts("Adding Work:");

  // Move File Ptr to Beg
  fseek(works->fp, 0, SEEK_SET);

  // Initializing Buffers
  char course[MAX_COURSE_LEN];
  char name[MAX_NAME_LEN];

  // Newline Pointer to Remove Newline
  char *newline;

  // Get Time
  printf("Which course?: ");
  fgets(course, MAX_COURSE_LEN, stdin);
  fflush(stdin);

  if ((newline = strchr(course, '\n'))) {
    *newline = '\0';
  }

  // Get Work Name
  printf("What's its name?: ");
  fgets(name, MAX_NAME_LEN, stdin);
  fflush(stdin);

  if ((newline = strchr(name, '\n'))) {
    *newline = '\0';
  }
  char due_date_out[MAX_FILE_DATE_LEN];
  struct tm due_date_tm;
  if (get_date(due_date_out, &due_date_tm) == EXIT_FAILURE) {
    return;
  }

  char confirmation[CONFIRMATION_LEN];
  printf("Add work: %s in %s Class, due on %s? [Y/n]: ", name, course,
         due_date_out);
  fgets(confirmation, CONFIRMATION_LEN, stdin);
  fflush(stdin);

  if (tolower(confirmation[0]) != 'y') {
    puts("cancelling");
    puts("=========================================================");
    return;
  }

  time_t now;
  time(&now);
  if (difftime(mktime(&due_date_tm), now) < 0) {
    printf("Are you sure you want to continue, %s is in the past? [Y/n]: ",
           due_date_out);
    fgets(confirmation, CONFIRMATION_LEN, stdin);
    fflush(stdin);

    if (tolower(confirmation[0]) != 'y') {
      puts("cancelling");
      puts("=========================================================");
      return;
    }
  }

  // Set File Pointer to EOF
  fseek(works->fp, 0, SEEK_END);

  // Format Date into New Row
  char out[MAX_ROW_LEN];
  sprintf(out, "%s,%s,%s,False\n", course, name, due_date_out);

  // Write New Row to File
  fwrite(out, sizeof(char), strlen(out), works->fp);

  // Increment Row Len
  works->len_rows++;
  puts("=========================================================");
}
