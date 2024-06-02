#include <windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <cstddef>
#include "common.h"


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

void paste(bool newline, bool isUTF8, std::string watch_cmd = "") {
	std::string clipboard{};

	// if watch argument is set
	if (watch_cmd != "") {
		//execProgWithArgs(watch_cmd, false);
		
		create_watch([]() {std::cout << "hahaha" << std::endl; });
		exit(0);
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


	return 0;
}