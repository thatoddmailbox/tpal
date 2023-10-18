#ifndef _TPAL_PRIVATE_DIALOG_LINUX_H
#define _TPAL_PRIVATE_DIALOG_LINUX_H

#include "tpal/dialog.h"

typedef struct {
	void (*init)();

	char * (*open_file)(const char * title, TpalFileDialogOptions * options);
	char * (*save_file)(const char * title, TpalFileDialogOptions * options);
} tpal_dialog_dispatch_t;

tpal_dialog_dispatch_t dispatch_linux_dummy;
tpal_dialog_dispatch_t dispatch_linux_gtk3;

#endif
