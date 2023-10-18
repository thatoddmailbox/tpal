#include <stdlib.h>
#include <stdio.h>

#include "tpal/init.h"
#include "tpal/dialog.h"

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
	const char * text_extensions[] = {"*.txt", NULL};
	const char * source_extensions[] = {"*.c", "*.cpp", "*.h", NULL};
	TpalDialogFilter filters[] = {
		{
			.name = "Text files",
			.extensions = text_extensions
		},
		{
			.name = "C/C++ source code",
			.extensions = source_extensions
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

	result = tpal_dialog_open_file(NULL, &options);
	printf("result: %s\n", result);
	if (result != NULL) {
		free(result);
	}

	//
	// save file, no filter options
	//
	result = tpal_dialog_save_file(NULL, NULL);
	printf("result: %s\n", result);
	if (result != NULL) {
		free(result);
	}

	return 0;
}