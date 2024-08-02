#include <string>
#include <Windows.h>
#include <iostream>
#include "common.h"


std::wstring convert_str_to_wstr(const std::string& utf8string, const int CODE_PAGE) {
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
		print_error("Failed to get wideSzie.",ERRCODE_CONVERT_WIDE_SIZE_FAILED);
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

std::string convert_wstr_to_str(std::wstring& wstr, const int code_page) {

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

int execute_program_args(std::string command, bool wait, std::string stdin_data) {
    std::wstring ws = convert_str_to_wstr(command, CP_ACP);
    LPWSTR lpCommandLine = const_cast<LPWSTR>(ws.data());

    PROCESS_INFORMATION pi;
    ZeroMemory(&pi, sizeof(pi));

    STARTUPINFO si;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);

    // Set up the standard input pipe for the new process
    SECURITY_ATTRIBUTES sa;
    sa.nLength = sizeof(sa);
    sa.bInheritHandle = TRUE;
    sa.lpSecurityDescriptor = NULL;

    HANDLE hInputRead, hInputWrite;
    if (!CreatePipe(&hInputRead, &hInputWrite, &sa, 0)) {
        print_error("CreatePipe failed.");
        return -1;
    }

    // Ensure the write handle to the pipe is not inherited.
    if (!SetHandleInformation(hInputWrite, HANDLE_FLAG_INHERIT, 0)) {
        print_error("SetHandleInformation failed.");
        CloseHandle(hInputRead);
        CloseHandle(hInputWrite);
        return -1;
    }

    si.hStdInput = hInputRead;
    si.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    si.hStdError = GetStdHandle(STD_ERROR_HANDLE);
    si.dwFlags |= STARTF_USESTDHANDLES;

    BOOL result = CreateProcessW(
        NULL,
        lpCommandLine,
        NULL,
        NULL,
        TRUE,
        0,
        NULL,
        NULL,
        &si,
        &pi
    );

    if (!result) {
        print_error("CreateProcess failed.");
        CloseHandle(hInputRead);
        CloseHandle(hInputWrite);
        return -1;
    }

    // Close the read handle in the parent process
    CloseHandle(hInputRead);

    // Write to the write end of the pipe
    DWORD written;
    if (!WriteFile(hInputWrite, stdin_data.c_str(), static_cast<DWORD>(stdin_data.length()), &written, NULL)) {
        print_error("WriteFile to pipe failed.");
        CloseHandle(hInputWrite);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        return -1;
    }

    // Close the write end of the pipe
    CloseHandle(hInputWrite);

    if (wait) {
        WaitForSingleObject(pi.hProcess, INFINITE);
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return 0;
}

// print raw byte to stdout without any transfer('\n' to '\r\n')
int write_raw_data_to_stdout(std::string& s) {
	HANDLE hOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hOutput == INVALID_HANDLE_VALUE) {
		print_error("Error getting handle to output.");
		return 1;
	}

	DWORD written;
    if (!WriteFile(hOutput, s.c_str(), static_cast<DWORD>(s.size()), &written, nullptr)) {
		print_error("Error writing to output."); 
		return 1;
	}

	return 0;
}