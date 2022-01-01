#ifndef _TPAL_PRIVATE_DIALOG_LINUX_H
#define _TPAL_PRIVATE_DIALOG_LINUX_H

typedef struct {
	void (*init)();

	char * (*open_file)();
} tpal_dialog_dispatch_t;

tpal_dialog_dispatch_t dispatch_linux_dummy;
tpal_dialog_dispatch_t dispatch_linux_gtk3;

#endif