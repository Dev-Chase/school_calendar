#include "delete.h"
#include <ctype.h>
#include <string.h>

#define TEMP_DEL_NAME "/scal-temp-del.txt"
void delete_work(const char *cal_path, DF works[static 1], const char *opt) {
	puts("=========================================================");

	// Create Path to Temp File
	char temp_path[strlen(cal_path) + strlen(TEMP_DEL_NAME) + 1];
	create_scal_path(temp_path, cal_path, TEMP_DEL_NAME);

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
	uint_fast32_t sel_row = select_row(works, opt);
	if (sel_row == 0) {
		puts("no works match that name, cancelling");
		puts("=========================================================");
		return;
	}

	// Set File Ptr to Beg
	fseek(works->fp, 0, SEEK_SET);

	char row_buff[MAX_ROW_LEN + 1]; // +1 for '\n'
	// Loop Through Until sel_row
	// (no need for -1 bc of column row)
	uint_fast32_t i;
	for (i = 0; i < sel_row; i++) {
		fgets(row_buff, MAX_ROW_LEN + 1, works->fp);
		fwrite(row_buff, 1, strlen(row_buff), temp_fp);
	}

	// Confirm Deletion
	fgets(row_buff, MAX_ROW_LEN+1, works->fp);
	char *course = row_buff;
	char *name = strchr(row_buff, ',') + 1;
	char *due_date = strchr(name, ',') + 1;
	char *submitted = strchr(due_date, ',') + 1;

	printf("Delete ");
	print_field(name);
	printf(" in ");
	print_field(course);
	printf(" Class, due ");
	print_field(due_date);
	printf(" and %s submitted? [Y/n]: ",
		   (submitted[0] == 'F') ? "not" : "already");

	char confirmation;
	scanf("%c", &confirmation);
	fflush(stdin);
	if (tolower(confirmation) != 'y') {
		puts("ok, cancelling");
		fwrite(row_buff, 1, strlen(row_buff), temp_fp);
	}

	// Add Back the Rest of the Rows
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

	// Decrement Row Len if Confirmed
	if (tolower(confirmation) == 'y') {
		works->len_rows--;
	}

	puts("it's done");
	puts("=========================================================");
}
