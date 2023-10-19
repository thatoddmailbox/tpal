#include <stdio.h>

#include <windows.h>

#define COBJMACROS
#include <combaseapi.h>
#include <shlobj.h>
#include <commdlg.h>

#include "tpal/dialog.h"

#include "base.h"

static bool initialized = false;

static PWSTR convert_utf8_to_wchar(const char * utf8_str) {
	// first, figure out how long the utf8 string has to be
	int size = MultiByteToWideChar(
		CP_UTF8,
		0,
		utf8_str,
		-1,
		NULL,
		0
	);

	PWSTR result = malloc(size * sizeof(WCHAR));

	// now, actually do the conversion
	MultiByteToWideChar(
		CP_UTF8,
		0,
		utf8_str,
		-1,
		result,
		size
	);

	return result;
}

static PWSTR convert_utf8_to_wchar_com(const char * utf8_str) {
	// first, figure out how long the utf8 string has to be
	int size = MultiByteToWideChar(
		CP_UTF8,
		0,
		utf8_str,
		-1,
		NULL,
		0
	);

	PWSTR result = CoTaskMemAlloc(size * sizeof(WCHAR));

	// now, actually do the conversion
	MultiByteToWideChar(
		CP_UTF8,
		0,
		utf8_str,
		-1,
		result,
		size
	);

	return result;
}

static char * convert_wchar_to_utf8(PWSTR wchar_str) {
	// first, figure out how long the utf8 string has to be
	int size = WideCharToMultiByte(
		CP_UTF8,
		0,
		wchar_str,
		-1,
		NULL,
		0,
		NULL,
		NULL
	);

	char * result = malloc(size * sizeof(char));

	// now, actually do the conversion
	WideCharToMultiByte(
		CP_UTF8,
		0,
		wchar_str,
		-1,
		result,
		size,
		NULL,
		NULL
	);

	return result;
}

static char * tpal_dialog_windows_helper(REFCLSID rclsid, const char * title, TpalFileDialogOptions * options) {
	IFileDialog * pfd = NULL;
	HRESULT hr = CoCreateInstance(
		rclsid,
		NULL,
		CLSCTX_INPROC_SERVER,
		&IID_IFileDialog,
		&pfd
	);
	if (!SUCCEEDED(hr)) {
		if (pfd != NULL) {
			IFileDialog_Release(pfd);
			pfd = NULL;
		}
		return NULL;
	}

	if (options != NULL && options->suggested_name != NULL) {
		PWSTR wide_file_path = convert_utf8_to_wchar(options->suggested_name);

		PWSTR wide_filename = wcsrchr(wide_file_path, '\\');
		if (wide_filename == NULL) {
			// no slash - it's just a filename
			wide_filename = wide_file_path;
		} else {
			// has slash - we have to find the parent folder and set it

			// need to resolve parent folder to IShellItem

			// convert file path into just parent folder
			wide_filename[0] = '\0';

			IShellItem * parent_folder;

			// TODO: SHCreateItemFromParsingName requires vista or newer
			// can fix with https://stackoverflow.com/a/20887104/2178519
			// (but supporting XP or earlier is mostly for the memes I think)
			hr = SHCreateItemFromParsingName(wide_file_path, NULL, &IID_IShellItem, &parent_folder);
			if (!SUCCEEDED(hr)) {
				return NULL;
			}

			// restore slash to overall path
			wide_filename[0] = '\\';

			// remove slash from filename string
			wide_filename = wide_filename + 1;

			// now, we can finally actually set it
			hr = IFileDialog_SetFolder(pfd, parent_folder);
			if (!SUCCEEDED(hr)) {
				return NULL;
			}

			IShellItem_Release(parent_folder);
		}

		hr = IFileDialog_SetFileName(pfd, wide_filename);
		if (!SUCCEEDED(hr)) {
			return NULL;
		}

		free(wide_file_path);
	}

	if (options != NULL && options->filters != NULL)  {
		int filter_count = 0;
		TpalDialogFilter * filter = options->filters;
		while (filter->extensions != NULL) {
			filter_count++;
			filter++;
		}

		if (options->allow_all_files) {
			filter_count++;
		}

		COMDLG_FILTERSPEC * windows_filters = CoTaskMemAlloc(filter_count * sizeof(COMDLG_FILTERSPEC));

		const PCWSTR all_files_name = L"All Files";

		COMDLG_FILTERSPEC * windows_filter = windows_filters;
		filter = options->filters;
		while (filter->extensions != NULL) {
			windows_filter->pszName = convert_utf8_to_wchar_com(filter->name);

			// windows wants the extensions in a single string, separated by semicolons
			// for example "*.png; *.jpg; *.jpeg; *.gif"

			// first, calculate how long our string should be
			size_t spec_string_length = 0;
			bool first = true;

			const char ** extension = filter->extensions;
			while (*extension) {
				if (first) {
					first = false;
				} else {
					// semicolon
					spec_string_length++;

					// space
					spec_string_length++;
				}

				spec_string_length += strlen(*extension);

				extension++;
			}

			// now, actually build the string
			char * spec_string = malloc((spec_string_length + 1) * sizeof(char));
			char * spec_string_pos = spec_string;

			extension = filter->extensions;
			first = true;
			while (*extension) {
				if (first) {
					first = false;
				} else {
					// semicolon
					*spec_string_pos = ';';
					spec_string_pos++;

					// space
					*spec_string_pos = ' ';
					spec_string_pos++;
				}

				size_t n = strlen(*extension);
				memcpy(spec_string_pos, *extension, n * sizeof(char));
				spec_string_pos += n;

				extension++;
			}
			*spec_string_pos = '\0';

			windows_filter->pszSpec = convert_utf8_to_wchar_com(spec_string);
			free(spec_string);

			windows_filter++;
			filter++;
		}

		if (options->allow_all_files) {
			windows_filter->pszName = all_files_name;
			windows_filter->pszSpec = L"*.*";
		}

		hr = IFileDialog_SetFileTypes(pfd, filter_count, windows_filters);
		if (!SUCCEEDED(hr)) {
			return NULL;
		}

		windows_filter = windows_filters;
		for (int i = 0; i < filter_count; i++) {
			if (windows_filter->pszName == all_files_name) {
				// it's a constant, not heap allocated
				windows_filter++;
				continue;
			}

			CoTaskMemFree((PWSTR) windows_filter->pszName);
			CoTaskMemFree((PWSTR) windows_filter->pszSpec);

			windows_filter++;
		}

		CoTaskMemFree(windows_filters);
	}

	hr = IFileDialog_Show(pfd, NULL);
	if (!SUCCEEDED(hr)) {
		return NULL;
	}

	IShellItem * result;
	hr = IFileDialog_GetResult(pfd, &result);
	if (!SUCCEEDED(hr)) {
		return NULL;
	}

	PWSTR wide_file_path = NULL;
	hr = IShellItem_GetDisplayName(result, SIGDN_FILESYSPATH, &wide_file_path);
	if (!SUCCEEDED(hr)) {
		return NULL;
	}

	IShellItem_Release(result);

	char * file_path = convert_wchar_to_utf8(wide_file_path);

	CoTaskMemFree(wide_file_path);

	IFileDialog_Release(pfd);

	return file_path;
}

TPAL_PUBLIC_API char * tpal_dialog_open_file(const char * title, TpalFileDialogOptions * options) {
	if (!initialized) {
		return NULL;
	}

	return tpal_dialog_windows_helper(&CLSID_FileOpenDialog, title, options);
}

TPAL_PUBLIC_API char * tpal_dialog_save_file(const char * title, TpalFileDialogOptions * options) {
	if (!initialized) {
		return NULL;
	}

	return tpal_dialog_windows_helper(&CLSID_FileSaveDialog, title, options);
}

void tpal_dialog_init() {
	initialized = true;

	// IFileDialog requires COM, set that up now
	// note S_FALSE is OK as a result because it means someone else initialized COM for us
	// TODO: think about this more in the context of someone else also using COM
	HRESULT res = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	if (res != S_OK && res != S_FALSE) {
		// TODO: how to report this?
		printf("CoInitializeEx failed with %d\n", res);
		return;
	}
}