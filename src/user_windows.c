#include "tpal/user.h"

#include <stdio.h>

#include <windows.h>

#include <ShlObj.h>

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

	// note: path always has no trailing backslash

	// arguments: path, vendor, vendor slash, program
	const char * format_str = "%s\\%s%s%s";

	int full_path_length = snprintf(NULL, 0, format_str, path, (vendor ? vendor : ""), ((vendor && program) ? "\\" : "") , (program ? program : ""));
	char * full_path = malloc(full_path_length + 1);
	snprintf(full_path, full_path_length + 1, format_str, path, (vendor ? vendor : ""), ((vendor && program) ? "\\" : "") , (program ? program : ""));

	free(path);

	return full_path;
}