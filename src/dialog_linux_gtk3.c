#include <stddef.h>
#include <stdio.h>

#include "dialog_linux.h"

char * tpal_dialog_linux_gtk3_file_open() {
	printf("tpal_dialog_linux_gtk3_file_open\n");
	return NULL;
}

tpal_dialog_dispatch_t dispatch_linux_gtk3 = {
	.open_file = tpal_dialog_linux_gtk3_file_open	
};