#include <stddef.h>

#include "dialog_linux.h"

char * tpal_dialog_linux_dummy_file_open() {
	return NULL;
}

tpal_dialog_dispatch_t dispatch_linux_dummy = {
	.open_file = tpal_dialog_linux_dummy_file_open	
};