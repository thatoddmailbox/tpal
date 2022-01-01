#include <stdbool.h>
#include <stdio.h>

#include "base.h"
#include "dialog_linux.h"

bool initialized = false;
tpal_dialog_dispatch_t active_dispatch;

TPAL_PUBLIC_API char * tpal_dialog_open_file() {
	if (!initialized) {
		return;
	}

	return active_dispatch.open_file();
}

void tpal_dialog_init() {
	active_dispatch = dispatch_linux_gtk3;
	initialized = true;
}