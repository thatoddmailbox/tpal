#ifndef _TPAL_DIALOG_H
#define _TPAL_DIALOG_H

#include <stdbool.h>

typedef struct {
	const char * name;
	const char ** extensions;
} TpalDialogFilter;

typedef struct {
	bool allow_all_files;
	TpalDialogFilter * filters;
} TpalDialogFilterOptions;

char * tpal_dialog_open_file(const char * title, TpalDialogFilterOptions * filter_options);

#endif