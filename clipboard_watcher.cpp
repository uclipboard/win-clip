#include <Windows.h>
#include <iostream>
#include <cassert>
#include <functional>

// ok guys, it is too hard to understand for me now...

static const int shake_interval = 200;//ms
static const WCHAR* clipboardWatcherTitle = L"WinClipClipboardWatcher";
static std::function<void()>  on_clipboard_changed;

// main window procedure funcion
static LRESULT CALLBACK windows_procedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
	case WM_CLIPBOARDUPDATE:
		KillTimer(hwnd, 1);
		SetTimer(hwnd, 1, shake_interval, nullptr);
		break;

	case WM_TIMER:
		//disappear shakes!
		assert(on_clipboard_changed != nullptr);
		on_clipboard_changed();
		KillTimer(hwnd, 1);

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
	wc.lpszClassName = clipboardWatcherTitle;
	if (!RegisterClassW(&wc)) {
		std::cerr << "register window failed." << std::endl;
		return 1;
	}

	// create windowless window
	HWND hwnd = CreateWindowExW(0, clipboardWatcherTitle, NULL, 0, 0, 0, 0, 0, NULL, NULL, NULL, NULL);

	if (!hwnd) {
		std::cerr << "create window failed." << std::endl;
		return 1;
	}
	AddClipboardFormatListener(hwnd);
	// msg loop
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}

int create_watch(std::function<void()> func_callback) {
	on_clipboard_changed = func_callback;
	return create_windowsless_window();
};
