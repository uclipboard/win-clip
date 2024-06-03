#include <string>
#include <Windows.h>
#include <iostream>
std::wstring convert_str_to_wstr(const std::string& utf8string, const int CODE_PAGE = CP_UTF8) {
	if (utf8string.empty()) return std::wstring();

	int wc_size = MultiByteToWideChar(
		CODE_PAGE,
		0,
		utf8string.c_str(),
		-1,
		nullptr,
		0
	);

	if (wc_size == 0) {
		std::cerr << "Failed to get wideSzie" << std::endl;
		return std::wstring();
	}

	std::wstring wstr(wc_size, 0);
	MultiByteToWideChar(
		CODE_PAGE,
		0,
		utf8string.c_str(),
		-1,
		&wstr[0],
		wc_size
	);

	return wstr;
}

std::string convert_wstr_to_str(std::wstring& wstr, const int code_page = CP_ACP) {

	auto cs = wstr.c_str();
	// it caculate out the c-style string end '\0'
	auto UTF8_str_len = WideCharToMultiByte(code_page, 0, cs, -1, nullptr, 0, nullptr, nullptr);
	std::string s(UTF8_str_len, 0);
	if (UTF8_str_len > 0) {

		WideCharToMultiByte(code_page, 0, cs, -1, &s[0], UTF8_str_len, nullptr, nullptr);
	}
	if (!s.empty() && s.back() == '\0') {
		s.resize(s.size() - 1);
	}
	return s;
}

int execute_program_args(std::string command, bool UTF8_in, bool wait = false) {
	std::wstring ws;
	if (UTF8_in) {
		ws = convert_str_to_wstr(command, CP_UTF8);
	}
	else {
		ws = convert_str_to_wstr(command, CP_ACP);
	}
	LPWSTR lpCommandLine = new WCHAR[ws.length() + 1]();
	wcscpy_s(lpCommandLine, ws.length() + 1, ws.c_str());

	PROCESS_INFORMATION pi;
	ZeroMemory(&pi, sizeof(pi));

	STARTUPINFO si;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);

	if (!CreateProcess(NULL, 
		lpCommandLine,
		NULL, 
		NULL, 
		FALSE,
		0, 
		NULL, 
		NULL, 
		&si, 
		&pi
	)) {

		std::cerr << "CreateProcess failed (" << GetLastError() << ")\n";
		delete[] lpCommandLine;
		return -1;
	}

	if (wait) {
		WaitForSingleObject(pi.hProcess, INFINITE);
	}
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	delete[] lpCommandLine;

	return 0;
}