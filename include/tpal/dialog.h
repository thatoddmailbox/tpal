#ifndef _TPAL_DIALOG_H
#define _TPAL_DIALOG_H

#ifdef __cplusplus
extern "C" {
#endif

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
char * tpal_dialog_save_file(const char * title, TpalDialogFilterOptions * filter_options);

#ifdef __cplusplus
}
#endif

#endif