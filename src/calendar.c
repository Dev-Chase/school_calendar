#include "calendar.h"
#include <errno.h>
#include "list.h"
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

void create_scal_path(char *dest, const char *cal_path, const char *fname) {
	strcpy(dest, cal_path);
	strcat(dest, fname);
}

bool cmp_field(const char *field, const char *cmp_field) {
	size_t i = 0;
	while (field[i] != ',' && cmp_field[i] != ',' && field[i] && cmp_field[i]) {
		if (field[i] != cmp_field[i]) {
			return false;
		}
		i++;
	}
	if ((field[i] != ',' && field[i] != '\0') || (cmp_field[i] != ',' && cmp_field[i] != '\0')) {
		return false;
	}
	return true;
}

bool cmp_time(char *row, const char *cmp_time) {
	char *token = row;

	// Move Token Ptr to Due Date Field
	for (size_t i = 0; i < DUE_COL; i++) {
		token = strchr(token, ',') + 1;
	}

	// Copy Due Date Data to due_str
	char due_str[MAX_DATE_LEN - 1 + strlen(" - ") + MAX_TIME_LEN];
	size_t i = 0;
	while (token[i] != ',') {
		due_str[i] = token[i];
		i++;
	}
	due_str[i] = '\0';

	// Parse due_str
	struct tm due_date;
	strptime(due_str, "%d %B %Y - %H:%M", &due_date);
	due_date.tm_sec = 0;

	// Get Current Time
	time_t now;
	time(&now);

	// Make Token = Submitted field
	token = strchr(token, ',') + 1;
	return difftime(mktime(&due_date), now) < (atoi(cmp_time) * 3600 * 24) &&
		   !strcmp(token, "False");
}

void print_field(const char *field) {
	for (size_t i = 0; field[i] != ',' && field[i] != '\0'; i++) {
		printf("%c", field[i]);
	}
}

bool in_arr(size_t cmp_i, size_t len,
			const size_t arr[static len]) {
	for (size_t i = 0; i < len; i++) {
		if (cmp_i == arr[i]) {
			return true;
		}
	}
	return false;
}

size_t select_row(DF works[static 1], const char *opt) {
	char input[MAX_NAME_LEN];
	if (opt == NULL) {
		list_works(works, "a");
		printf("Which work is it (name or index)?: ");
		fgets(input, MAX_NAME_LEN, stdin);
		fflush(stdin);
	} else {
		strcpy(input, opt);
	}

	char *newline;
	if ((newline = strchr(input, '\n'))) {
		*newline = '\0';
	}
	size_t out;
	char row_buff[MAX_ROW_LEN];
	if ((out = atoi(input)) == 0) {
		// Move FP Down One Line from Beg
		fseek(works->fp, 0, SEEK_SET);
		fscanf(works->fp, "%*[^\n]\n");

		size_t matching_len = 0;
		size_t *matching =
			(size_t *)malloc(sizeof(size_t) * works->len_rows);

		for (size_t i = 0; i < works->len_rows; i++) {
			fgets(row_buff, MAX_ROW_LEN, works->fp);

			if ((newline = strchr(row_buff, '\n'))) {
				*newline = '\0';
			}

			if (cmp_field(strchr(row_buff, ',') + 1, input)) {
				matching[matching_len] = i + 1;
				matching_len++;
			}
		}

		if (matching_len == 0) {
			out = 0;
		} else if (matching_len == 1) {
			out = matching[0];
		} else {
			// Move File Ptr 1 Line Down from Beg
			fseek(works->fp, 0, SEEK_SET);
			fscanf(works->fp, "%*[^\n]\n");

			for (size_t i = 0, x = 0; i < works->len_rows; i++) {
				fgets(row_buff, MAX_ROW_LEN, works->fp);
				if (i + 1 == matching[x]) {
					if ((newline = strchr(row_buff, '\n')) != NULL) {
						*newline = '\0';
					}

					char *course = row_buff;
					char *name = strchr(course, ',') + 1;
					char *due_date = strchr(name, ',') + 1;
					char *submitted = strchr(due_date, ',') + 1;

					printf("\n%zu - ", matching[x]);
					print_field(name);
					printf(" in ");
					print_field(course);
					printf(" Class, due ");
					print_field(due_date);
					if (!strcmp(submitted, "True")) {
						puts("\nIt's already submitted.");
					} else if (!strcmp(submitted, "False") &&
							   cmp_time(row_buff, "0")) {
						puts("\nIt's passed due!");
					} else {
						puts("\nIt's not submitted yet.");
					}

					x++;
				}
			}
			printf("There are multiple works matching that name, which index "
				   "is it?: ");
			scanf("%zu", &out);
			fflush(stdin);

			if (!in_arr(out, matching_len, matching)) {
				puts("invalid index, cancelling");
				out = 0;
			}
		}

		free(matching);
	} else {
		size_t *range =
			(size_t *)malloc(sizeof(size_t) * works->len_rows);
		for (size_t i = 0; i < works->len_rows; i++) {
			range[i] = i + 1;
		}
		if (!in_arr(out, works->len_rows, range)) {
			puts("invalid index, cancelling");
			out = 0;
		}
		free(range);
	}

	if (out != 0) {
		// Set File Ptr one line down from beg
		fseek(works->fp, 0, SEEK_SET);
		fscanf(works->fp, "%*[^\n]\n");

		// Loop Through Until out row
		for (size_t i = 0; i < out; i++) {
			fgets(row_buff, MAX_ROW_LEN, works->fp);
		}
		const char *name = strchr(row_buff, ',') + 1;
		const char *due_date = strchr(name, ',') + 1;
		const char *submitted = strchr(due_date, ',')+1;

		printf("Selected: ");
		print_field(name);
		printf(" in ");
		print_field(row_buff);
		printf(" class, %s submitted and due ", (!strcmp(submitted, "False")) ? "not" : "already");
		print_field(due_date);
		puts("");
	}

	return out;
}

void open_data_frame(char *cal_path, DF *works) {
	if (mkdir(cal_path, S_IRWXU | S_IRWXG | S_IROTH) != 0 && errno != EEXIST) {
		fprintf(stderr, "error creating %s folder", cal_path);
		perror("");
	}

	// Create Path to Works Data Frame
	char works_path[strlen(cal_path) + strlen(WORKS_NAME) + 1];
	create_scal_path(works_path, cal_path, WORKS_NAME);

	// Open Event File
	works->fp = fopen(works_path, "a+");
	if (works->fp == NULL) {
		fprintf(stderr, "error: failed to open %s, aborting\n", works_path);
		abort();
	}

	// Move File Ptr to EOF
	fseek(works->fp, 0, SEEK_END);

	// Add Columns if not already there
	if (ftell(works->fp) == 0) {
		char *columns = "Course,Name,Due Date,Submitted\n";
		fwrite(columns, 1, strlen(columns), works->fp);
	}

	// Move File Ptr to Beg
	fseek(works->fp, 0, SEEK_SET);

	// Get Number of Lines
	works->len_rows = 0;
	while (fscanf(works->fp, "%*[^\n]\n") != EOF) {
		works->len_rows++;
	}

	// Remove One for Columns
	works->len_rows--;
}

int8_t get_date(char out[static MAX_FILE_DATE_LEN], struct tm *date_out) {
	char due_date_str[MAX_DATE_LEN + strlen(" - ") + MAX_TIME_LEN];
	char due_time[MAX_TIME_LEN];
	char *newline;

	// Get Due Date
	printf("What day is it due? (ex: July 10, 2023): ");
	fgets(due_date_str, MAX_DATE_LEN, stdin);
	fflush(stdin);

	if ((newline = strchr(due_date_str, '\n'))) {
		*newline = '\0';
	}

	// Get Due Time
	printf("At what time? (ex: 23:59): ");
	fgets(due_time, MAX_TIME_LEN, stdin);
	fflush(stdin);
	if ((newline = strchr(due_time, '\n'))) {
		*newline = '\0';
	}

	// Append Time to Date
	strcat(due_date_str, " - ");
	strcat(due_date_str, due_time);

	// Reformat Due Date for File
	if (!strptime(due_date_str, "%B %d, %Y - %H:%M", date_out)) {
		fprintf(stderr, "error: invalid date or time format\ncancelling\n");
		puts("=========================================================");
		return EXIT_FAILURE;
	}
	strftime(out, MAX_FILE_DATE_LEN, "%d %B %Y - %H:%M", date_out);
	date_out->tm_sec = 0;
	return EXIT_SUCCESS;
}
