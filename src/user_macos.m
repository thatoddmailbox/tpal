#include "tpal/user.h"

#include <string.h>

#include <AppKit/AppKit.h>

char * tpal_user_get_temp_path() {
	return strdup("/tmp/");
}

void tpal_user_shell_open(const char * path) {
	[[NSWorkspace sharedWorkspace] openFile:[NSString stringWithUTF8String:path]];
}