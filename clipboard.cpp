#include <Windows.h>
#include <iostream>
#include <string>
#include "common.h"

#define LAST_ERROR_CODE "("<<GetLastError()<<")"

const static int DEFAULT_RETRY_NUMBER = 5;
const static int DEFAULT_RETRY_DELAY_TIME = 100;

static inline HANDLE retry_SetClipboardData(UINT uFormat,HANDLE hGlobal,int retry_time = DEFAULT_RETRY_NUMBER) {
	for (int i = 0; i < retry_time; i++) {
		auto ret = SetClipboardData(uFormat, hGlobal);
		if (ret != nullptr) return ret;
		Sleep(DEFAULT_RETRY_DELAY_TIME);
	}
	return nullptr;
}

static inline BOOL retry_OpenClipboard(HWND hWndNewOwner,int retry_time = DEFAULT_RETRY_NUMBER) {
	for (int i = 0; i < retry_time; i++) {
		auto ret = OpenClipboard(hWndNewOwner);
		if (ret) return ret;
		Sleep(DEFAULT_RETRY_DELAY_TIME);
	}
	return false;
}

static inline HANDLE retry_GetClipboardData(UINT uFormat, int retry_time = DEFAULT_RETRY_NUMBER) {
	for (int i = 0; i < retry_time; i++) {
		auto ret = GetClipboardData(uFormat);
		if (ret != nullptr) return ret;
		Sleep(DEFAULT_RETRY_DELAY_TIME);
	}
	return nullptr;
	
}

int copy_UTF8_to_clipboard(std::string& msg) {

	auto wmsg = convert_str_to_wstr(msg);
	auto data = wmsg.c_str();
	auto data_size = wmsg.size() + 1;

	HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, data_size * sizeof(WCHAR));
	if (!hGlobal) {
		std::cerr << "Failed to allocate memory"<< LAST_ERROR_CODE << std::endl;
		return 1;
	}

	LPWSTR pGlobal = static_cast<LPWSTR>(GlobalLock(hGlobal));
	if (!pGlobal) {
		std::cerr << "Failed to lock memory" << LAST_ERROR_CODE << std::endl;
		GlobalFree(hGlobal);
		return 1;
	}

	wcscpy_s(pGlobal, data_size, data);
	GlobalUnlock(hGlobal);

	if (!retry_OpenClipboard(nullptr)) {
		std::cerr << "Failed to open clipboard" << LAST_ERROR_CODE << std::endl;
		return 1;
	}

	EmptyClipboard();


	if (retry_SetClipboardData(CF_UNICODETEXT, hGlobal) == nullptr) {
		std::cerr << "Failed to set clipboard data" << LAST_ERROR_CODE << std::endl;
		GlobalFree(hGlobal);
		CloseClipboard();
		return 1;
	}

	CloseClipboard();
	return 0;

}
int copy_ANSI_to_clipboard(std::string& msg) {

	auto data = msg.c_str();
	auto data_size = strlen(data) + 1;

	HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, data_size);
	if (!hGlobal) {
		std::cerr << "Failed to allocate memory" << LAST_ERROR_CODE << std::endl;
		return 1;
	}

	LPSTR pGlobal = static_cast<LPSTR>(GlobalLock(hGlobal));
	if (!pGlobal) {
		std::cerr << "Failed to lock memory" << LAST_ERROR_CODE << std::endl;
		GlobalFree(hGlobal);
		return 1;
	}

	memcpy(pGlobal, data, data_size);
	GlobalUnlock(hGlobal);

	if (!retry_OpenClipboard(nullptr)) {
		std::cerr << "Failed to open clipboard" << LAST_ERROR_CODE << std::endl;
		return 1;
	}

	EmptyClipboard();

	if (retry_SetClipboardData(CF_UNICODETEXT, hGlobal) == nullptr) {
		std::cerr << "Failed to set clipboard data"<< LAST_ERROR_CODE << std::endl;
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
	if (hData == nullptr) {
		std::cerr << "There is no data in the clipboard." << std::endl;
		return 1;
	}
	// Lock the clipboard data to get a pointer to it
	LPSTR cbText = static_cast<LPSTR>(GlobalLock(hData));
	if (cbText == nullptr) {
		std::cerr << "Unable to lock the clipboard data." << std::endl;
		return 1;
	}
	s = cbText;
	GlobalUnlock(hData);
	return 0;
}
// wchar content
int get_clipboard_content(HANDLE& hData, std::wstring& s) {
	// Lock the clipboard data to get a pointer to it
	LPWSTR wcbText = static_cast<LPWSTR>(GlobalLock(hData));
	if (wcbText == nullptr) {
		std::cerr << "Unable to lock the clipboard data." << std::endl;
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
	if (!retry_OpenClipboard(nullptr)) {
		std::cerr << "Unable to open the clipboard." << std::endl;
		return 1;
	}
	// Try to get the clipboard data as CF_UNICODETEXT first
	HANDLE hData = retry_GetClipboardData(CF_UNICODETEXT);
	if (hData == nullptr) {
		// If we can't get CF_UNICODETEXT, try getting CF_TEXT
		call_return = get_clipboard_content(s);
	}
	else {

		std::wstring ws;
		call_return = get_clipboard_content(hData, ws);
		if (isUTF8) {
			s = convert_wstr_to_str(ws, CP_UTF8);
		}
		else {
			s = convert_wstr_to_str(ws);
		}
	}

	// Close the clipboard
	CloseClipboard();
	return call_return;
}
