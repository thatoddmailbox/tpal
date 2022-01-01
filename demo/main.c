#include <stdlib.h>
#include <stdio.h>

#include "init.h"
#include "dialog.h"

int main(int argc, const char * argv[])
{
	printf("tpal demo\n");

	tpal_init();

	char * result = tpal_dialog_open_file();
	printf("result: %s\n", result);
	if (result != NULL) {
		free(result);
	}

	return 0;
}