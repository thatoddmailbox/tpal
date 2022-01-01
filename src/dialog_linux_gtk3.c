#include <stddef.h>
#include <stdio.h>

#include <dlfcn.h>

#include <gtk/gtk.h>

#include "dialog_linux.h"

void * libgtk3_handle;

void (*_gtk_init)(int * argc, char *** argv);
GtkWidget * (*_gtk_file_chooser_dialog_new)(const gchar * title, GtkWindow * parent, GtkFileChooserAction action, const gchar * first_button_text, ...);
gint (*_gtk_dialog_run)(GtkDialog * dialog);
void (*_gtk_widget_destroy)(GtkWidget * widget);

void tpal_dialog_linux_gtk3_init() {
	libgtk3_handle = dlopen("libgtk-3.so", RTLD_LAZY);

	// TODO: error handling

	_gtk_init = dlsym(libgtk3_handle, "gtk_init");
	_gtk_file_chooser_dialog_new = dlsym(libgtk3_handle, "gtk_file_chooser_dialog_new");
	_gtk_dialog_run = dlsym(libgtk3_handle, "gtk_dialog_run");
	_gtk_widget_destroy = dlsym(libgtk3_handle, "gtk_widget_destroy");

	_gtk_init(NULL, NULL);
}

char * tpal_dialog_linux_gtk3_open_file() {
	GtkWidget * dialog = _gtk_file_chooser_dialog_new(
		"Open File",
		NULL,
		GTK_FILE_CHOOSER_ACTION_OPEN,
		"_Cancel",
		GTK_RESPONSE_CANCEL,
		"_Open",
		GTK_RESPONSE_ACCEPT,
		NULL
	);
	gint result = _gtk_dialog_run((GtkDialog *) dialog);
	_gtk_widget_destroy(dialog);
	return NULL;
}

tpal_dialog_dispatch_t dispatch_linux_gtk3 = {
	.init = tpal_dialog_linux_gtk3_init,
	.open_file = tpal_dialog_linux_gtk3_open_file
};