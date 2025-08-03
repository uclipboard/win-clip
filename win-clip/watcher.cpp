#include <Windows.h>
#include <iostream>
#include <cassert>
#include <functional>
#include "common.h"
#include <WtsApi32.h>


// ok guys, it is too hard to understand for me now...

static const int SHAKE_RANGE[] = { 100, 200 };//ms
static const WCHAR* CLIPBOARD_WATCHER_TITLE = L"WinClipClipboardWatcher";
static std::function<void()>  on_clipboard_changed;
static std::function<void()> on_screen_unlocked;
// main window procedure funcion
static LRESULT CALLBACK windows_procedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	bool delay_trigger = true;
	switch (message) {
	case WM_CLIPBOARDUPDATE:
		KillTimer(hwnd, 1);
		//to avoid repeated clipboard update
		SetTimer(hwnd, 1, random_int(SHAKE_RANGE[0], SHAKE_RANGE[1]), NULL);
		break;

	case WM_TIMER:
		assert(on_clipboard_changed != NULL);
		on_clipboard_changed();
		KillTimer(hwnd, 1);
		break;
	case WM_WTSSESSION_CHANGE:
		if (wParam == WTS_SESSION_UNLOCK) {
			assert(on_screen_unlocked != nullptr);
			on_screen_unlocked();
		}
		break;
	default:
		return DefWindowProc(hwnd, message, wParam, lParam);
	}
	return 0;
}
static int create_windowsless_window() {
	WNDCLASSW wc = { 0 };
	wc.lpfnWndProc = windows_procedure;
	wc.hInstance = GetModuleHandle(NULL);
	wc.lpszClassName = CLIPBOARD_WATCHER_TITLE;
	if (!RegisterClassW(&wc)) {
		print_error("Register window failed.");
		return 1;
	}

	// create windowless window
	HWND hwnd = CreateWindowExW(0, CLIPBOARD_WATCHER_TITLE, NULL, 0, 0, 0, 0, 0, NULL, NULL, NULL, NULL);

	if (!hwnd) {
		print_error("Create window failed.");
		return 1;
	}
	AddClipboardFormatListener(hwnd);
	
	print_log("Clipboard format listener added.");

	if (!WTSRegisterSessionNotification(hwnd, NOTIFY_FOR_THIS_SESSION)) {
		print_error("Register session notification failed.");
	}

	print_log("Clipboard watcher window created successfully.");

	// msg loop
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	WTSUnRegisterSessionNotification(hwnd);

	return (int)msg.wParam;
}

static void screen_unlock_notify() {
	print_error("Screen unlocked.", ERRCODE_SCREEN_UNLCOKED_NOTIFY);
}

int create_watch(std::function<void()> func_callback) {
	on_clipboard_changed = func_callback;
	on_screen_unlocked = screen_unlock_notify;
	return create_windowsless_window();
};
