#include <stdlib.h>
#include <stdio.h>

#include "init.h"
#include "dialog.h"

int main(int argc, const char * argv[])
{
	printf("tpal demo\n");

	tpal_init();

	//
	// open file, no filter options
	//
	char * result = tpal_dialog_open_file(NULL, NULL);
	printf("result: %s\n", result);
	if (result != NULL) {
		free(result);
	}

	//
	// open file, filter options
	//
	const char * text_extensions[] = {".txt", NULL};
	TpalDialogFilter filters[] = {
		{
			.name = "Text files",
			.extensions = text_extensions
		},
		{
			.name = NULL,
			.extensions = NULL
		}
	};
	TpalDialogFilterOptions options = {
		.allow_all_files = false,
		.filters = filters
	};

	result = tpal_dialog_open_file(NULL, NULL);
	printf("result: %s\n", result);
	if (result != NULL) {
		free(result);
	}

	return 0;
}