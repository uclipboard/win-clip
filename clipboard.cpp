#include <Windows.h>
#include <iostream>
#include <string>
#include "common.h"

const static int DEFAULT_RETRY_NUMBER = 50;
const static int DEFAULT_RETRY_DELAY_TIME = 200;


static inline bool try_fix_error(DWORD error);

static inline HANDLE retry_SetClipboardData(UINT uFormat, HANDLE hGlobal, int retry_time = DEFAULT_RETRY_NUMBER) {
	for (int i = 0; i < retry_time; i++) {
		auto ret = SetClipboardData(uFormat, hGlobal);
		if (ret != NULL) {
			print_log("success");
			return ret;
		}
		try_fix_error(GetLastError());
		print_log("retry");
		Sleep(DEFAULT_RETRY_DELAY_TIME);
	}
	return NULL;
}

static inline BOOL retry_OpenClipboard(HWND hWndNewOwner, int retry_time = DEFAULT_RETRY_NUMBER) {
	for (int i = 0; i < retry_time; i++) {
		auto ret = OpenClipboard(hWndNewOwner);
		if (ret)
		{
			print_log("success");
			return ret;
		}
		try_fix_error(GetLastError());
		print_log("retry");
		Sleep(DEFAULT_RETRY_DELAY_TIME);
	}
	return false;
}

static inline HANDLE retry_GetClipboardData(UINT uFormat, int retry_time = DEFAULT_RETRY_NUMBER) {
	for (int i = 0; i < retry_time; i++) {
		auto ret = GetClipboardData(uFormat);
		if (ret != NULL) {
			print_log("success");
			return ret;
		}
		try_fix_error(GetLastError());
		print_log("retry");
		Sleep(DEFAULT_RETRY_DELAY_TIME);
	}
	return NULL;

}

static inline bool retry_is_clipboard_has_text(int retry_time = DEFAULT_RETRY_NUMBER) {
	for (int i = 0; i < retry_time; i++) {
		if (IsClipboardFormatAvailable(CF_UNICODETEXT) || IsClipboardFormatAvailable(CF_TEXT))
		{
			print_log("success");
			return true;
		}
		try_fix_error(GetLastError());
		print_log("retry");
		Sleep(DEFAULT_RETRY_DELAY_TIME);
	}
	return false;
}

static inline bool try_fix_error(DWORD error) {
	switch (error) {
	case ERROR_CLIPBOARD_NOT_OPEN:
		// reopen clipboard
		if (retry_OpenClipboard(NULL)) {
			print_log(": reopen clipboard");
			return true;
		}
	}
	// format the last error message
#ifdef _DEBUG
	LPVOID lpMsgBuf{};
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		error,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf,
		0, NULL);
	print_log("try fix error failed");
	print_log((char*)lpMsgBuf);
#endif
	return false;
}


int copy_data_to_clipboard(std::wstring& wmsg) {

	auto data = wmsg.c_str();
	auto data_size = wmsg.size() + 1;

	HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, data_size * sizeof(WCHAR));
	if (!hGlobal) {
		print_error("Failed to allocate memory.");
		return 1;
	}

	LPWSTR pGlobal = static_cast<LPWSTR>(GlobalLock(hGlobal));
	if (!pGlobal) {
		print_error("Failed to lock memory.");
		GlobalFree(hGlobal);
		return 1;
	}

	wcscpy_s(pGlobal, data_size, data);
	GlobalUnlock(hGlobal);

	if (!retry_OpenClipboard(NULL)) {
		print_error("Failed to open clipboard.");
		return 1;
	}

	EmptyClipboard();


	if (retry_SetClipboardData(CF_UNICODETEXT, hGlobal) == NULL) {
		print_error("Failed to set clipboard data.");
		GlobalFree(hGlobal);
		CloseClipboard();
		return 1;
	}

	CloseClipboard();
	return 0;

}
int copy_data_to_clipboard(std::string& msg) {

	auto data = msg.c_str();
	auto data_size = strlen(data) + 1;

	HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, data_size);
	if (!hGlobal) {
		print_error("Failed to allocate memory.");
		return 1;
	}

	LPSTR pGlobal = static_cast<LPSTR>(GlobalLock(hGlobal));
	if (!pGlobal) {
		print_error("Failed to lock memory.");
		GlobalFree(hGlobal);
		return 1;
	}

	memcpy(pGlobal, data, data_size);
	GlobalUnlock(hGlobal);

	if (!retry_OpenClipboard(NULL)) {
		print_error("Failed to open clipboard.");
		return 1;
	}

	EmptyClipboard();

	if (retry_SetClipboardData(CF_TEXT, hGlobal) == NULL) {
		print_error("Failed to set clipboard data.");
		GlobalFree(hGlobal);
		CloseClipboard();
		return 1;
	}

	CloseClipboard();
	return 0;
}


// ascii content
int get_clipboard_content(std::string& s) {
	HANDLE hData = retry_GetClipboardData(CF_TEXT);
	if (hData == NULL) {
		print_error("Unable to read clipboard data as plain text.");
		return 1;
	}
	// Lock the clipboard data to get a pointer to it
	LPSTR cbText = static_cast<LPSTR>(GlobalLock(hData));
	if (cbText == NULL) {
		print_error("Unable to lock the clipboard data.");
		return 1;
	}
	s = cbText;
	GlobalUnlock(hData);
	return 0;
}

// wchar content
int get_clipboard_content(std::wstring& s) {
	HANDLE hData = retry_GetClipboardData(CF_UNICODETEXT);
	if (hData == NULL) {
		print_error("Unable to read clipboard data as unicode text.");
		return 1;
	}
	// Lock the clipboard data to get a pointer to it
	LPWSTR wcbText = static_cast<LPWSTR>(GlobalLock(hData));
	if (wcbText == NULL) {
		print_error("Unable to lock the clipboard data.");
		return 1;

	}
	s = wcbText;
	// Unlock the clipboard data
	GlobalUnlock(hData);
	return 0;
}



int paste_from_clipboard(std::string& s, bool isUTF8) {

	int call_return{};
	// Open the clipboard
	if (!retry_OpenClipboard(NULL)) {
		print_error("Unable to open the clipboard.");
		return 1;
	}
	else if (!retry_is_clipboard_has_text()) {
		print_error("Can't get right format clipboard", ERRCODE_CLIPBOARD_EMPTY);
		call_return = 1;
	}
	else if (IsClipboardFormatAvailable(CF_UNICODETEXT)) {
		std::wstring ws;
		call_return = get_clipboard_content(ws);
		if (isUTF8) {
			s = convert_wstr_to_str(ws, CP_UTF8);
		}
		else {
			s = convert_wstr_to_str(ws);
		}
	}
	else if (IsClipboardFormatAvailable(CF_TEXT)) {
		call_return = get_clipboard_content(s);
	}
	else {
		print_error("Unable to recognise the clipboard data type.", ERRCODE_CLIPBOARD_DATA_TYPE_UNKNOWN);
		call_return = 1;
	}

	CloseClipboard();
	return call_return;
}
