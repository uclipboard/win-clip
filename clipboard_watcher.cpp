#include <Windows.h>
#include <iostream>
#include <cassert>
#include <functional>

// ok guys, it is too hard to understand for me now...


static const WCHAR* clipboardWatcherTitle = L"WinClipClipboardWatcher";
static std::function<void()>  on_clipboard_changed;

// main window procedure funcion
static LRESULT CALLBACK windows_procedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
	case WM_CLIPBOARDUPDATE:

		assert(on_clipboard_changed != nullptr);
		on_clipboard_changed();
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
		// In this process, some received but not handled msgs may be dropped by PeekMessage, 
		// but the clupboard update event isn't a high frequency event.So I'd better take it easy
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)); //drop the msgs received during processing of current msg
	}

	return (int)msg.wParam;
}

int create_watch(std::function<void()> func_callback) {
	on_clipboard_changed = func_callback;
	return create_windowsless_window();
};
