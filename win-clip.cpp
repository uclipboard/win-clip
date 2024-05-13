#include <windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <cstddef>

const std::string copy_suffix{ "win-copy" };
const std::string paste_suffix{ "win-paste" };
const std::string ext_execute{ ".exe" };


void help() {
	std::cout << "win-clip (https://github.com/dangjinghao/win-clip): A part of uclipboard." << std::endl;
	std::cout << "Usage: win-clip [MODE] [OPTION] ..." << std::endl;
	std::cout << "options:" << std::endl;
	std::cout << "-h\t display this help and exit." << std::endl;
	std::cout << "-n\t print message from system clipboard with newline in `paste` mode." << std::endl;
	std::cout << "-m\t pass message you want to copy to system clipboard in `copy` mode." << std::endl;
	std::cout << "-u\t print or receive content will be treated as UTF8 encoded if this flag was specified in `paste` mode." << std::endl;
	std::cout << std::endl << "example:" << std::endl;
	std::cout << "win-clip -h \t display help." << std::endl;
	std::cout << "win-clip copy -m hello world \t copy 'hello world' to system clipboard." << std::endl;
	std::cout << "win-clip copy \t copy the message readed from stdin to system clipboard." << std::endl;
	std::cout << "or: echo hello world | win-clip copy \t copy hello world to system clipboard." << std::endl;
	std::cout << std::endl;
	std::cout << "win-clip paste \t print the latest message from system clipboard." << std::endl;
	std::cout << "win-clip paste -n \t print the latest message from system clipboard with newline." << std::endl;
	std::cout << "By the way, win-clip supports alias name, "
		"so you can hard or soft link to to some special name and use it in this way:" << std::endl;
	std::cout << std::endl;
	std::cout << "> mklink.exe /H win-paste.exe win-clip.exe" << std::endl;
	std::cout << "> mklink.exe /H win-copy.exe win-clip.exe" << std::endl;
	std::cout << std::endl;
	std::cout << "win-paste == win-clip paste and win-copy == win-clip copy" << std::endl;
	std::cout << "echo hello world | win-copy" << std::endl;
	std::cout << "win-copy -m hello world" << std::endl;
	std::cout << "win-paste [-h]" << std::endl;

	exit(0);
}

std::wstring utf8_to_wstr(const std::string& utf8string) {
	if (utf8string.empty()) return std::wstring();

	int wideSize = MultiByteToWideChar(
		CP_UTF8,
		0,
		utf8string.c_str(),
		-1,
		nullptr,
		0
	);

	if (wideSize == 0) {
		std::cerr << "Failed to get wideSzie" << std::endl;
		return std::wstring();
	}

	std::wstring wstr(wideSize, 0); // 预分配宽字符字符串  
	MultiByteToWideChar(
		CP_UTF8,
		0,
		utf8string.c_str(),
		-1,
		&wstr[0],         // 输出缓冲区  
		wideSize         // 宽度  
	);

	return wstr;
}

#define FAIL() {CloseClipboard();return 1;}
int copy_UTF8_to_clipboard(std::string& msg) {

	if (!OpenClipboard(nullptr)) {
		std::cerr << "Failed to open clipboard" << std::endl;
		return 1;
	}

	EmptyClipboard();
	auto wmsg = utf8_to_wstr(msg);
	auto data = wmsg.c_str();
	size_t dataSize = wcslen(data) + 1;

	HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, dataSize * sizeof(wchar_t));
	if (!hGlobal) {
		std::cerr << "Failed to allocate memory" << std::endl;
		FAIL();
	}

	wchar_t* pGlobal = static_cast<wchar_t*>(GlobalLock(hGlobal));
	if (!pGlobal) {
		std::cerr << "Failed to lock memory" << std::endl;
		GlobalFree(hGlobal);
		FAIL();
	}

	wcscpy_s(pGlobal, dataSize, data);
	GlobalUnlock(hGlobal);


	if (SetClipboardData(CF_UNICODETEXT, hGlobal) == nullptr) {
		std::cerr << "Failed to set clipboard data" << std::endl;
		GlobalFree(hGlobal);
		FAIL();
	}

	CloseClipboard();
	return 0;

}
int copy_ANSI_to_clipboard(std::string& msg) {
	if (!OpenClipboard(nullptr)) {
		std::cerr << "Failed to open clipboard" << std::endl;
		return 1;
	}

	EmptyClipboard();
	auto data = msg.c_str();
	size_t dataSize = strlen(data) + 1;

	HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, dataSize);
	if (!hGlobal) {
		std::cerr << "Failed to allocate memory" << std::endl;
		FAIL();
	}

	LPSTR pGlobal = static_cast<LPSTR>(GlobalLock(hGlobal));
	if (!pGlobal) {
		std::cerr << "Failed to lock memory" << std::endl;
		GlobalFree(hGlobal);
		FAIL();
	}

	memcpy(pGlobal, data, dataSize);
	GlobalUnlock(hGlobal);


	if (SetClipboardData(CF_TEXT, hGlobal) == nullptr) {
		std::cerr << "Failed to set clipboard data" << std::endl;
		GlobalFree(hGlobal);
		FAIL();
	}

	CloseClipboard();
	return 0;
}
#undef FAIL


int paste_ascii_from_clipboard(std::string& s) {
	HANDLE hData = GetClipboardData(CF_TEXT);
	if (hData == nullptr) {
		std::cerr << "There is no data in the clipboard." << std::endl;
		return 1;
	}
	// Lock the clipboard data to get a pointer to it
	char* cbText = static_cast<char*>(GlobalLock(hData));
	if (cbText == nullptr) {
		std::cerr << "Unable to lock the clipboard data." << std::endl;
		return 1;
	}
	s = cbText;
	GlobalUnlock(hData);
	return 0;
}

std::string wstr_to_ANSI_str(std::wstring& wstr) {
	std::string s{};
	auto cs = wstr.c_str();
	// Convert the wide character string to ANSI
	char* mbText = nullptr;
	int mbLen = WideCharToMultiByte(CP_ACP, 0, cs, -1, nullptr, 0, nullptr, nullptr);
	if (mbLen > 0) {
		mbText = new char[mbLen];
		WideCharToMultiByte(CP_ACP, 0, cs, -1, mbText, mbLen, nullptr, nullptr);
		s = mbText;
		delete[] mbText;

	}
	return s;
}

std::string wstr_to_UTF8_str(std::wstring& wstr) {
	std::string s{};
	auto cs = wstr.c_str();
	// Convert the wide character string to UTF-8
	char* mbText = nullptr;
	int mbLen = WideCharToMultiByte(CP_UTF8, 0, cs, -1, nullptr, 0, nullptr, nullptr);
	if (mbLen > 0) {
		mbText = new char[mbLen];
		WideCharToMultiByte(CP_UTF8, 0, cs, -1, mbText, mbLen, nullptr, nullptr);
		s = mbText;
		delete[] mbText;
	}
	return s;
}



int paste_wchars_from_clipboard(HANDLE& hData, std::wstring& s) {
	// Lock the clipboard data to get a pointer to it
	wchar_t* wcbText = static_cast<wchar_t*>(GlobalLock(hData));
	if (wcbText == nullptr) {
		std::cerr << "Unable to lock the clipboard data." << std::endl;
		return 1;

	}
	s = wcbText;
	// Unlock the clipboard data
	GlobalUnlock(hData);
	return 0;
}

std::string paste_from_clipboard(bool isUTF8) {
	std::string s{};

	// Open the clipboard
	if (!OpenClipboard(nullptr)) {
		std::cerr << "Unable to open the clipboard." << std::endl;
		return s;
	}
	// Try to get the clipboard data as CF_UNICODETEXT first
	HANDLE hData = GetClipboardData(CF_UNICODETEXT);
	if (hData == nullptr) {
		// If we can't get CF_UNICODETEXT, try getting CF_TEXT

		paste_ascii_from_clipboard(s);
	}
	else {

		std::wstring ws;
		paste_wchars_from_clipboard(hData, ws);
		if (isUTF8) {
			s = wstr_to_UTF8_str(ws);
		}
		else {
			s = wstr_to_ANSI_str(ws);
		}
	}

	// Close the clipboard
	CloseClipboard();
	return s;
}



void copy(std::string& arg_msg, bool isUTF8IN) {
	std::string a_msg;
	if (arg_msg.empty()) {
		std::string in_str{ (std::istreambuf_iterator<char>(std::cin)),std::istreambuf_iterator<char>() };
		a_msg = in_str;
	}
	else {
		a_msg = arg_msg;
	}
	int ret;
	if (isUTF8IN) {
		ret = copy_UTF8_to_clipboard(a_msg);
	}
	else {
		ret = copy_ANSI_to_clipboard(a_msg);

	}
	exit(ret);
}
void paste(bool newline, bool isUTF8) {
	std::cout << paste_from_clipboard(isUTF8);
	if (newline) {
		std::cout << std::endl;
	}
	else {
		std::cout << std::flush;
	}
	exit(0);
}

std::string prog_name;
std::string sub_command;
std::string msg_opt;
bool newline_opt;
bool UTF8IO_opt;
bool help_opt;

int main(int argc, char* argv[]) {
	std::vector<std::string> args;

	for (int i = 0; i < argc; ++i)
		args.push_back(std::string(argv[i]));


	prog_name = args[0];

	if (prog_name.ends_with(ext_execute)) {
		prog_name.erase(prog_name.length() - ext_execute.size(), ext_execute.size());
	}

	if (prog_name.ends_with(copy_suffix)) {
		args.insert(args.begin() + 1, "copy");
	}
	else if (prog_name.ends_with(paste_suffix)) {
		args.insert(args.begin() + 1, "paste");
	}

	if (args.size() == 1) {
		help();
	}

	sub_command = args[1];
	// prog_name sub_command -opt
	for (int i = 2; i < args.size(); i++) {
		std::string arg = args[i];
		if (arg == "-h") help_opt = true;
		else if (arg == "-u") UTF8IO_opt = true;
		else if (arg == "-n") newline_opt = true;
		else if (arg == "-m") {
			for (int j = i + 1; j < args.size(); j++) {
				if (j != i + 1)
					msg_opt.append(" ");

				msg_opt.append(args[j]);
			}

			break;
		}
	}

	if (help_opt)
		help();

	if (sub_command == "copy") {
		copy(msg_opt, UTF8IO_opt);
	}
	else if (sub_command == "paste") {
		paste(newline_opt, UTF8IO_opt);
	}
	help();
	return 0;
}