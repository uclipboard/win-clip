#include <windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <cstddef>
#include <functional>
#include "common.h"

static const int reset_time = 200;

void copy(std::string& arg_msg, bool isUTF8IN) {
	std::string clipboard_data;
	if (arg_msg.empty()) {
		// I don't know why should I use c++ style like that!!
		std::string in_str{ (std::istreambuf_iterator<char>(std::cin)),std::istreambuf_iterator<char>() };
		clipboard_data = in_str;
	}
	else {
		clipboard_data = arg_msg;
	}
	int ret;
	if (isUTF8IN) {
		ret = copy_UTF8_to_clipboard(clipboard_data);
	}
	else {
		ret = copy_ANSI_to_clipboard(clipboard_data);

	}
	exit(ret);
}

volatile bool _open = 0;


std::function<void()> clipboard_update_action(std::string watch_cmd) {
	return [watch_cmd]() {
		// mask!
		if (!_open) {
			_open = true;
			execute_program_args(watch_cmd, false, true);
		}
		};
}

DWORD WINAPI open_reset(LPVOID lpParameter) {
	// mask!
	while (true) {
		if (_open) {
			_open = false;
		}
		Sleep(reset_time);
	}
	return 0;
}

void paste(bool newline, bool isUTF8, std::string watch_cmd = "") {
	std::string clipboard{};

	// if watch argument is set
	if (watch_cmd != "") {
		// clipboard open and close, it will be called twice
		// https://stackoverflow.com/questions/10373713/clipboard-listener-event-is-being-called-twice

		HANDLE hThread = CreateThread(NULL, 0, open_reset, NULL, 0, NULL);

		if (hThread == NULL) {
			std::cerr << "CreateThread failed (" << GetLastError() << ")" << std::endl;
			exit(1);
		}

		exit(
			create_watch(clipboard_update_action(watch_cmd))
		);
	}

	// else paste as usual

	int ret = paste_from_clipboard(clipboard, isUTF8);

	if (ret) {
		exit(ret);
	}
	std::cout << clipboard;
	if (newline) {
		std::cout << std::endl;
	}
	else {
		std::cout << std::flush;
	}

	exit(0);
}

int main(int argc, char* argv[]) {

	parser parser(argc, argv);


	if (parser.help_opt) {
		parser.help();
		exit(0);
	}

	if (parser.sub_command == "copy") copy(parser.msg_opt, parser.UTF8IO_opt);
	else if (parser.sub_command == "paste") paste(parser.newline_opt, parser.UTF8IO_opt, parser.watch_cmd);

	parser.help();
	return 0;
}