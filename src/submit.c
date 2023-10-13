#include "submit.h"
#include <ctype.h>
#include <string.h>

#define TEMP_SUB_NAME "/scal-temp-submit.txt"
void submit_work(const char *cal_path, const char *opt, DF works[static 1]) {
	puts("=========================================================");

	// Create Path to Temp File
	char temp_path[strlen(cal_path) + strlen(TEMP_SUB_NAME) + 1];
	create_scal_path(temp_path, cal_path, TEMP_SUB_NAME);

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

	// Ask Whether the User Should Submit or Unsubmit
	char sub_or_unsub;
	printf("Do you want to Submit or Unsubmit?: [S/u] ");
	scanf("%c", &sub_or_unsub);
	fflush(stdin);
	char *replacement = (tolower(sub_or_unsub) == 's') ? "True\n" : "False\n";

	// Find Work Row
	size_t sel_row = select_row(works, opt);
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
	size_t i;
	for (i = 0; i < sel_row; i++) {
		fgets(row_buff, MAX_ROW_LEN + 1, works->fp);
		fwrite(row_buff, 1, strlen(row_buff), temp_fp);
	}

	// Change Selected Row
	fgets(row_buff, MAX_ROW_LEN + 1, works->fp);
	char *old_submit = strrchr(row_buff, ',') + 1;
	if (old_submit[0] == replacement[0]) {
		printf("it's already %s, cancelling\n",
			   (old_submit[0] == 'T') ? "submitted" : "not submitted");
		fwrite(row_buff, 1, strlen(row_buff), temp_fp);
	} else {
		fwrite(row_buff, 1, (strrchr(row_buff, ',') + 1) - row_buff, temp_fp);
		fwrite(replacement, 1, strlen(replacement), temp_fp);
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

	puts("=========================================================");
}
