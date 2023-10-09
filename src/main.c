#include "add.h"
#include "delete.h"
#include "list.h"
#include "change.h"
#include "submit.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define MAX_INP 22 // strlen("remove") + strlen("not submitted") + \0

int main(int argc, char **argv) {
	// Opening Default Calendar Folder
	printf("using default calendar directory ($HOME/scal)\n");
	char *home = getenv("HOME");
	if (home == NULL) {
		fprintf(stderr, "error: $HOME is not set, aborting\n");
		abort();
	}
	char cp[strlen(home) + strlen("/scal") + 1];
	strcpy(cp, home);
	strcat(cp, "/scal");

	// Initializing Works Data Frame
	DF works;

	// Initialize and/or Create Data Frame
	open_data_frame(cp, &works);

	puts("Starting Scal:");
	puts("-----------------------------");

	char func[MAX_INP];
	char *sep;
	char *opt;
	while (true) {
		opt = NULL;
		printf(": ");
		fgets(func, MAX_INP, stdin);
		fflush(stdin);

		if ((sep = strchr(func, '\n')) != NULL) {
			*sep = '\0';
		}

		if ((sep = strchr(func, ' ')) != NULL) {
			*sep = '\0';
			opt = sep + 1;
		}

		if (!strcmp(func, "break") || !strcmp(func, "br") ||
			!strcmp(func, "b")) {
			break;
		}

		if (!strcmp(func, "c") || !strcmp(func, "cl") || !strcmp(func, "cls") ||
			!strcmp(func, "clr") || !strcmp(func, "clear")) {
			// Clear Screen Using Regex
			printf("\e[1;1H\e[2J");
		} else if (!strcmp(func, "l") || !strcmp(func, "list")) {
			// Listing Works Based on Option
			list_works(&works, opt);
		} else if (!strcmp(func, "a") || !strcmp(func, "all")) {
			add_work(&works);
		} else if (!strcmp(func, "d") || !strcmp(func, "rm") ||
				   !strcmp(func, "del") || !strcmp(func, "rem") ||
				   !strcmp(func, "delete") || !strcmp(func, "remove")) {
			delete_work(cp, &works, opt);
		} else if (!strcmp(func, "s") || !strcmp(func, "sub") ||
				   !strcmp(func, "submit")) {
			submit_work(cp, opt, &works);
		} else if (!strcmp(func, "cd") || !strcmp(func, "date") || !strcmp(func, "change_date")) {
			change_work_date(cp, &works, opt);
		} else if (!strcmp(func, "cn") || !strcmp(func, "name") || !strcmp(func, "change_name")) {
			change_work_name(cp, &works, opt);
		} else {
			fprintf(stderr, "error: invalid option\n");
		}
	}
	puts("-----------------------------");
	puts("Closing Scal");

	// Cleanup Calendar Folder
	fclose(works.fp);

	return 0;
}
