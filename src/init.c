#include "base.h"

#ifdef TPAL_PLATFORM_LINUX
#include <dlfcn.h>
#endif

void tpal_init() {
#ifdef TPAL_PLATFORM_LINUX
	// need to call XInitThreads, since GTK3 uses Xlib, and our user might also use Xlib
	// TODO: what we aren't using Xlib? what if the user doesn't even HAVE Xlib?
	void * libX11_handle = dlopen("libX11.so", RTLD_NOW);

	int (*_XInitThreads)();
	_XInitThreads = dlsym(libX11_handle, "XInitThreads");
	_XInitThreads();
	// note that we do NOT dlclose Xlib - otherwise, we might unlink from it and it goes away, which resets what we just did!
#endif

	tpal_dialog_init();
}