#include "tpal/user.h"

#include <stdio.h>

#include <windows.h>

#include <ShlObj.h>

// TODO: shouldn't duplicate this code between dialog_windows and here
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

// TODO: shouldn't duplicate this code between dialog_windows and here
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

char * tpal_user_get_app_data_path(const char * vendor, const char * program)
{
	// TODO: only supports vista and newer

	PWSTR wide_path;
	HRESULT hr = SHGetKnownFolderPath(&FOLDERID_RoamingAppData, 0, NULL, &wide_path);
	if (!SUCCEEDED(hr))
	{
		return NULL;
	}

	char * path = convert_wchar_to_utf8(wide_path);

	CoTaskMemFree(wide_path);

	// note: path from windows always has no trailing backslash
	// we want to return with a trailing slash

	// arguments: path, vendor, vendor slash, program
	const char * format_str = "%s\\%s%s%s\\";

	int full_path_length = snprintf(
		NULL,
		0,
		format_str,
		path,
		(vendor ? vendor : ""),
		((vendor && program) ? "\\" : ""),
		(program ? program : "")
	);
	char * full_path = malloc(full_path_length + 1);
	snprintf(
		full_path,
		full_path_length + 1,
		format_str,
		path,
		(vendor ? vendor : ""),
		((vendor && program) ? "\\" : ""),
		(program ? program : "")
	);

	free(path);

	// ensure the directory exists
	// (we ignore errors here bcause we don't care if it already exists, that's fine)
	// also note that SHCreateDirectory is recursive
	PWSTR wide_full_path = convert_utf8_to_wchar(full_path);
	SHCreateDirectory(NULL, wide_full_path);
	free(wide_full_path);

	return full_path;
}

void tpal_user_shell_open(const char * path) {
	PWSTR wide_path;
	if (path) {
		wide_path = convert_utf8_to_wchar(path);
	} else {
		// if null path, default to current working dir
		// TODO: does this make sense as an api?
		DWORD buffer_size = GetCurrentDirectoryW(0, NULL);
		wide_path = malloc(buffer_size * sizeof(WCHAR));
		GetCurrentDirectoryW(buffer_size, wide_path);
	}

	ShellExecuteW(NULL, L"open", wide_path, NULL, NULL, SW_SHOWNORMAL);

	free(wide_path);
}