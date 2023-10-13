#include "change.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#define TEMP_DATE_NAME "/scal-temp-date.txt"
void change_work_date(const char *cal_path, DF works[static 1], const char *opt) {
	puts("=========================================================");

	// Create Path to Temp File
	char temp_path[strlen(cal_path) + strlen(TEMP_DATE_NAME) + 1];
	create_scal_path(temp_path, cal_path, TEMP_DATE_NAME);

	// Get Path to Calendar DF
	char df_path[strlen(cal_path) + strlen(WORKS_NAME) + 1];
	create_scal_path(df_path, cal_path, WORKS_NAME);

	// Create Temporary File
	FILE *temp_fp = fopen(temp_path, "a+");

	// Make Sure Temp File doesn't have data
	fseek(temp_fp, 0, SEEK_END);
	if (ftell(temp_fp) != 0) {
		fprintf(stderr, "error: %s already exists and holds data, cancelling\n",
				temp_path);
		fclose(temp_fp);
		return;
	}

	// Find Work Row
	size_t sel_row = select_row(works, opt);
	if (sel_row == 0) {
		puts("no works match that name, cancelling");
		puts("=========================================================");
		return;
	}
	printf("getting new date\n\n");

	// Get the New Date
	char new_name[MAX_FILE_DATE_LEN];
	struct tm new_name_tm;
	if (get_date(new_name, &new_name_tm) == EXIT_FAILURE) {
		return;
	}

	time_t now;
	time(&now);
	if (difftime(mktime(&new_name_tm), now) < 0) {
		char confirmation[CONFIRMATION_LEN];
		printf("Are you sure you want to continue, %s is in the past? [Y/n]: ",
			   new_name);
		fgets(confirmation, CONFIRMATION_LEN, stdin);
		fflush(stdin);

		if (tolower(confirmation[0]) != 'y') {
			puts("cancelling");
			puts("=========================================================");

			remove(temp_path);
			return;
		}
	}

	// Set File Ptr to Beg
	fseek(works->fp, 0, SEEK_SET);

	char row_buff[MAX_ROW_LEN + 1]; // +1 for '\n'
	// Loop Through Until sel_row
	// (no need for -1 bc of column row)
	size_t i;
	for (i = 0; i < sel_row; i++) {
		fgets(row_buff, MAX_ROW_LEN + 1, works->fp);
		fwrite(row_buff, 1, strlen(row_buff), temp_fp);
	}

	// Change Selected Row
	fgets(row_buff, MAX_ROW_LEN + 1, works->fp);
	char *old_date = strchr(strchr(row_buff, ',') + 1, ',') + 1;
	if (cmp_field(old_date, new_name)) {
		puts("that's the same as the old date, cancelling");
		fwrite(row_buff, 1, strlen(row_buff), temp_fp);
	} else {
		fwrite(row_buff, 1, old_date - row_buff, temp_fp);
		fwrite(new_name, 1, strlen(new_name), temp_fp);
		fwrite(strrchr(row_buff, ','), 1, strlen(strrchr(row_buff, ',')),
			   temp_fp);
	}

	// Add Back the Rest of the Rows Normally
	for (i++; i < works->len_rows + 1; i++) {
		fgets(row_buff, MAX_ROW_LEN + 1, works->fp);
		fwrite(row_buff, 1, strlen(row_buff), temp_fp);
	}

	// Remove Old Dataframe File and Replace it with New One
	remove(df_path);
	rename(temp_path, df_path);

	// Close Old File Pointer and Replace it with New
	fclose(works->fp);
	works->fp = temp_fp;

	puts("it's done");
	puts("=========================================================");
}

#define TEMP_NAME_NAME "/scal-temp-name.txt"
void change_work_name(const char *cal_path, DF works[static 1], const char *opt) {
	puts("=========================================================");

	// Create Path to Temp File
	char temp_path[strlen(cal_path) + strlen(TEMP_NAME_NAME) + 1];
	create_scal_path(temp_path, cal_path, TEMP_NAME_NAME);

	// Get Path to Calendar DF
	char df_path[strlen(cal_path) + strlen(WORKS_NAME) + 1];
	create_scal_path(df_path, cal_path, WORKS_NAME);

	// Create Temporary File
	FILE *temp_fp = fopen(temp_path, "a+");

	// Make Sure Temp File doesn't have data
	fseek(temp_fp, 0, SEEK_END);
	if (ftell(temp_fp) != 0) {
		fprintf(stderr, "error: %s already exists and holds data, cancelling\n",
				temp_path);
		fclose(temp_fp);
		return;
	}

	// Find Work Row
	size_t sel_row = select_row(works, opt);
	if (sel_row == 0) {
		puts("no works match that name, cancelling");
		puts("=========================================================");
		return;
	}
	printf("getting new name\n\n");

	// Get the New Date
	char new_name[MAX_NAME_LEN];
	char *newline;
	printf("What's it's new name?: ");
	fgets(new_name, MAX_NAME_LEN, stdin);
	if ((newline = strchr(new_name, '\n')) != NULL) {
		*newline = '\0';
	}
	fflush(stdin);

	// Set File Ptr to Beg
	fseek(works->fp, 0, SEEK_SET);

	char row_buff[MAX_ROW_LEN + 1]; // +1 for '\n'
	// Loop Through Until sel_row
	// (no need for -1 bc of column row)
	size_t i;
	for (i = 0; i < sel_row; i++) {
		fgets(row_buff, MAX_ROW_LEN + 1, works->fp);
		fwrite(row_buff, 1, strlen(row_buff), temp_fp);
	}

	// Change Selected Row
	fgets(row_buff, MAX_ROW_LEN + 1, works->fp);
	char *old_name = strchr(row_buff, ',') + 1;
	char *due_date_start = strchr(old_name, ',');
	if (cmp_field(old_name, new_name)) {
		puts("that's the same as the old name, cancelling");
		fwrite(row_buff, 1, strlen(row_buff), temp_fp);
	} else {
		fwrite(row_buff, 1, old_name - row_buff, temp_fp);
		fwrite(new_name, 1, strlen(new_name), temp_fp);
		fwrite(due_date_start, 1, strlen(due_date_start), temp_fp);
	}

	// Add Back the Rest of the Rows Normally
	for (i++; i < works->len_rows + 1; i++) {
		fgets(row_buff, MAX_ROW_LEN + 1, works->fp);
		fwrite(row_buff, 1, strlen(row_buff), temp_fp);
	}

	// Remove Old Dataframe File and Replace it with New One
	remove(df_path);
	rename(temp_path, df_path);

	// Close Old File Pointer and Replace it with New
	fclose(works->fp);
	works->fp = temp_fp;

	puts("it's done");
	puts("=========================================================");
}
