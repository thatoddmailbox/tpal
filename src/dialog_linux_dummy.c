#include <stddef.h>

#include "dialog_linux.h"

void tpal_dialog_linux_dummy_init() {

}

char * tpal_dialog_linux_dummy_open_file(const char * title, TpalDialogFilterOptions * options) {
	return NULL;
}

tpal_dialog_dispatch_t dispatch_linux_dummy = {
	.init = tpal_dialog_linux_dummy_init,
	.open_file = tpal_dialog_linux_dummy_open_file
};