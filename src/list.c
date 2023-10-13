#include "list.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

bool cmp_submitted(char *row, const char *status) {
  char *token = row;
  for (size_t i = 0; i < SUB_COL; i++) {
    token = strchr(token, ',') + 1;
  }
  return !strcmp(token, status);
}

bool cmp_class_ns(char *row, const char *cmp_course) {
  return cmp_field(row, cmp_course) && !strcmp(strrchr(row, ',')+1, "False");
}

bool cmp_class_all(char *row, const char *cmp_course) {
  return cmp_field(row, cmp_course);
}

bool filter_none(char *_, const char *_2) { return true; }

void list_work(size_t i, char *row) {
  char *course = row;
  char *name = strchr(course, ',') + 1;
  char *due_date = strchr(name, ',') + 1;
  char *submitted = strchr(due_date, ',') + 1;

  time_t now;
  time(&now);

  struct tm dd;
  strptime(due_date, "%d %B %Y - %H:%M", &dd);
  dd.tm_sec = 0;

  double dif = difftime(mktime(&dd), now);
  size_t days = (size_t)(dif / 3600 / 24);
  size_t hours = (dif - days * 3600 * 24) / 3600;
  size_t mins = (dif - days * 3600 * 24 - hours * 3600) / 60;

  printf("\n%zu - ", i);
  print_field(name);
  printf(" in ");
  print_field(course);
  puts(" class");
  // printf("\n%u - %s in %s class\n", i, name, course);
  // printf("Due: %s", asctime(&dd));
  if (dif > 0 && strcmp(submitted, "True")) {
    puts("It isn't submitted yet.");
    printf("Due: in %zu days, %zu hours, and %zu minutes\v  -- %s", days, hours,
           mins, asctime(&dd));
  } else if (!strcmp(submitted, "True")) {
    puts("It's already submitted.");
  } else {
    puts("It's passed due!");
  }
}

void list_works(DF works[static 1], const char *opt) {
  puts("=========================================================");

  // Set File Ptr to Beg
  fseek(works->fp, 0, SEEK_SET);

  // Move File Pointer Down 1 Line
  fscanf(works->fp, "%*[^\n]\n");

  // Comparison Function that Takes in Row and Returns Bool
  bool (*cmp_fn)(char *, const char *);
  const char *cmp = "False";

  if (!opt || (!strcmp(opt, "ns") || !strcmp(opt, "not submitted"))) {
    puts("Listing Works that aren't Submitted:");
    cmp_fn = cmp_submitted;
  } else {
    if (!strcmp(opt, "a") || !strcmp(opt, "all")) {
      puts("Listing All Works:");
      cmp_fn = filter_none;
      cmp = NULL;
    } else if (!strcmp(opt, "s") || !strcmp(opt, "sub") ||
               !strcmp(opt, "submitted")) {
      puts("Listing Submitted Works:");
      cmp_fn = cmp_submitted;
      cmp = "True";
    } else if (!strcmp(opt, "e") || !strcmp(opt, "exp") ||
               !strcmp(opt, "expired")) {
      puts("Listing Expired Works:");
      cmp_fn = cmp_time;
      cmp = "0";
    } else if (!strcmp(opt, "ds") || !strcmp(opt, "due soon")) {
      puts("Listing Works that are Due Soon:");
      cmp_fn = cmp_time;
      cmp = "8";
    } else {
      if (opt[0] == 'a' && opt[1] == '-') {
        printf("Listing All Works in %s Class\n", opt+2);
        cmp_fn = cmp_class_all;
        cmp = opt + 2;
      } else {
        printf("Listing Works in %s Class\n", opt);
        cmp_fn = cmp_class_ns;
        cmp = opt;
      }
    }
  }

  // Number of Matching Rows
  size_t matching_n = 0;

  // Buffer to Hold Row Data
  char row_buff[MAX_ROW_LEN];

  char *sep;
  for (size_t i = 0; i < works->len_rows; i++) {
    fgets(row_buff, MAX_ROW_LEN, works->fp);
    fflush(stdin);

    if ((sep = strchr(row_buff, '\n')) != NULL) {
      *sep = '\0';
    }

    if (cmp_fn(row_buff, cmp)) {
      list_work(i + 1, row_buff);
      matching_n++;
    }
  }

  if (matching_n == 0) {
    puts("There are no works match that criteria.");
  }

  puts("=========================================================");
}
