#include <stdbool.h>
#include <stddef.h>

#include "dialog.h"

#include "base.h"
#include "dialog_linux.h"

bool initialized = false;
tpal_dialog_dispatch_t active_dispatch;

TPAL_PUBLIC_API char * tpal_dialog_open_file(const char * title, TpalDialogFilterOptions * filter_options) {
	if (!initialized) {
		return NULL;
	}

	return active_dispatch.open_file(title, filter_options);
}

TPAL_PUBLIC_API char * tpal_dialog_save_file(const char * title, TpalDialogFilterOptions * filter_options) {
	if (!initialized) {
		return NULL;
	}

	return active_dispatch.save_file(title, filter_options);
}

void tpal_dialog_init() {
	// TODO: actually decide this based on something
	active_dispatch = dispatch_linux_gtk3;

	active_dispatch.init();

	initialized = true;
}