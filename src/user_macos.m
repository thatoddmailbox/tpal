#include "tpal/user.h"

#include <string.h>

char * tpal_user_get_temp_path() {
	return strdup("/tmp/");
}

void tpal_user_shell_open(const char * path) {
	// TODO: implement
}