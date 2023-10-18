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
void (*_gtk_file_chooser_add_filter)(GtkFileChooser * chooser, GtkFileFilter * filte);
void (*_gtk_file_chooser_set_current_name)(GtkFileChooser * chooser, const gchar * current_name);
void (*_gtk_file_chooser_set_do_overwrite_confirmation)(GtkFileChooser * chooser, gboolean do_overwrite_confirmation);
GtkFileFilter * (*_gtk_file_filter_new)();
void (*_gtk_file_filter_add_pattern)(GtkFileFilter * filter, const gchar * pattern);
void (*_gtk_file_filter_set_name)(GtkFileFilter * filter, const gchar * name);
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
	_gtk_file_chooser_add_filter = dlsym(libgtk3_handle, "gtk_file_chooser_add_filter");
	_gtk_file_chooser_set_current_name = dlsym(libgtk3_handle, "gtk_file_chooser_set_current_name");
	_gtk_file_chooser_set_do_overwrite_confirmation = dlsym(libgtk3_handle, "gtk_file_chooser_set_do_overwrite_confirmation");
	_gtk_file_filter_new = dlsym(libgtk3_handle, "gtk_file_filter_new");
	_gtk_file_filter_add_pattern = dlsym(libgtk3_handle, "gtk_file_filter_add_pattern");
	_gtk_file_filter_set_name = dlsym(libgtk3_handle, "gtk_file_filter_set_name");
	_gtk_dialog_run = dlsym(libgtk3_handle, "gtk_dialog_run");
	_gtk_widget_destroy = dlsym(libgtk3_handle, "gtk_widget_destroy");

	_gtk_init(NULL, NULL);
}

static char * tpal_dialog_linux_gtk3_helper(GtkFileChooserAction action, const char * title, TpalFileDialogOptions * options) {
	GtkWidget * dialog = _gtk_file_chooser_dialog_new(
		title,
		NULL,
		action,
		"_Cancel",
		GTK_RESPONSE_CANCEL,
		(action == GTK_FILE_CHOOSER_ACTION_OPEN ? "_Open" : "_Save"),
		GTK_RESPONSE_ACCEPT,
		NULL
	);

	if (action == GTK_FILE_CHOOSER_ACTION_SAVE) {
		_gtk_file_chooser_set_do_overwrite_confirmation((GtkFileChooser *) dialog, TRUE);
	}

	if (options != NULL && options->suggested_name != NULL && action == GTK_FILE_CHOOSER_ACTION_SAVE) {
		// note that gtk only supports this for ACTION_SAVE and ACTION_CREATE_FOLDER
		_gtk_file_chooser_set_current_name((GtkFileChooser *) dialog, options->suggested_name);
	}

	if (options != NULL && options->filters != NULL) {
		TpalDialogFilter * filter = options->filters;
		while (filter->extensions != NULL) {
			GtkFileFilter * file_filter = _gtk_file_filter_new();
			const char ** extension = filter->extensions;
			while (*extension) {
				_gtk_file_filter_add_pattern(file_filter, *extension);
				extension++;
			}

			_gtk_file_filter_set_name(file_filter, filter->name);
			_gtk_file_chooser_add_filter((GtkFileChooser *) dialog, file_filter);

			filter++;
		}

		if (options->allow_all_files) {
			GtkFileFilter * all_files_filter = _gtk_file_filter_new();
			_gtk_file_filter_add_pattern(all_files_filter, "*");
			_gtk_file_filter_set_name(all_files_filter, "All files");
			_gtk_file_chooser_add_filter((GtkFileChooser *) dialog, all_files_filter);
		}
	}

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

char * tpal_dialog_linux_gtk3_open_file(const char * title, TpalFileDialogOptions * options) {
	return tpal_dialog_linux_gtk3_helper(GTK_FILE_CHOOSER_ACTION_OPEN, title, options);
}

char * tpal_dialog_linux_gtk3_save_file(const char * title, TpalFileDialogOptions * options) {
	return tpal_dialog_linux_gtk3_helper(GTK_FILE_CHOOSER_ACTION_SAVE, title, options);
}

tpal_dialog_dispatch_t dispatch_linux_gtk3 = {
	.init = tpal_dialog_linux_gtk3_init,
	.open_file = tpal_dialog_linux_gtk3_open_file,
	.save_file = tpal_dialog_linux_gtk3_save_file
};