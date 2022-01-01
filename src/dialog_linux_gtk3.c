#include <stddef.h>
#include <stdio.h>

#include <dlfcn.h>

#include <gtk/gtk.h>

#include "dialog_linux.h"

void * libglib2_handle;
void * libgtk3_handle;

void (*_g_free)(void * memory);

void (*_gtk_init)(int * argc, char *** argv);
GtkWidget * (*_gtk_file_chooser_dialog_new)(const gchar * title, GtkWindow * parent, GtkFileChooserAction action, const gchar * first_button_text, ...);
gchar * (*_gtk_file_chooser_get_filename)(GtkFileChooser * chooser);
gint (*_gtk_dialog_run)(GtkDialog * dialog);
void (*_gtk_widget_destroy)(GtkWidget * widget);

void tpal_dialog_linux_gtk3_init() {
	libglib2_handle = dlopen("libglib-2.0.so", RTLD_LAZY);
	libgtk3_handle = dlopen("libgtk-3.so", RTLD_LAZY);

	// TODO: error handling

	_g_free = dlsym(libglib2_handle, "g_free");

	_gtk_init = dlsym(libgtk3_handle, "gtk_init");
	_gtk_file_chooser_dialog_new = dlsym(libgtk3_handle, "gtk_file_chooser_dialog_new");
	_gtk_file_chooser_get_filename = dlsym(libgtk3_handle, "gtk_file_chooser_get_filename");
	_gtk_dialog_run = dlsym(libgtk3_handle, "gtk_dialog_run");
	_gtk_widget_destroy = dlsym(libgtk3_handle, "gtk_widget_destroy");

	_gtk_init(NULL, NULL);
}

char * tpal_dialog_linux_gtk3_open_file(const char * title, TpalDialogFilterOptions * filter_options) {
	GtkWidget * dialog = _gtk_file_chooser_dialog_new(
		title,
		NULL,
		GTK_FILE_CHOOSER_ACTION_OPEN,
		"_Cancel",
		GTK_RESPONSE_CANCEL,
		"_Open",
		GTK_RESPONSE_ACCEPT,
		NULL
	);

	char * result_filename = NULL;
	gint result = _gtk_dialog_run((GtkDialog *) dialog);
	if (result == GTK_RESPONSE_ACCEPT) {
		// while gtk_file_chooser_get_filename does give ownership to us, we're supposed to free the memory with g_free
		// the outside world doesn't know about g_free, so we copy it to a normal allocation that can be freed with free
		gchar * result_filename_g = _gtk_file_chooser_get_filename((GtkFileChooser *) dialog);
		result_filename = strdup(result_filename_g);
		_g_free(result_filename_g);
	}

	_gtk_widget_destroy(dialog);
	return result_filename;
}

tpal_dialog_dispatch_t dispatch_linux_gtk3 = {
	.init = tpal_dialog_linux_gtk3_init,
	.open_file = tpal_dialog_linux_gtk3_open_file
};