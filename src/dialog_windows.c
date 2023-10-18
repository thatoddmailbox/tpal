#include <stdio.h>

#include <windows.h>

#define COBJMACROS
#include <combaseapi.h>
#include <shlobj.h>
#include <commdlg.h>

#include "tpal/dialog.h"

#include "base.h"

static bool initialized = false;

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

	char * result = malloc(size);

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

TPAL_PUBLIC_API char * tpal_dialog_open_file(const char * title, TpalFileDialogOptions * filter_options) {
	if (!initialized) {
		return NULL;
	}

	IFileDialog * pfd = NULL;
	HRESULT hr = CoCreateInstance(
		&CLSID_FileOpenDialog,
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

	char * file_path = convert_wchar_to_utf8(wide_file_path);

	CoTaskMemFree(wide_file_path);

	IFileDialog_Release(pfd);

	return file_path;
}

TPAL_PUBLIC_API char * tpal_dialog_save_file(const char * title, TpalFileDialogOptions * filter_options) {
	if (!initialized) {
		return NULL;
	}

	// TODO: implement
	return NULL;
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