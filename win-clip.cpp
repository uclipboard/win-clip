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


std::function<void()> clipboard_update_action(std::string watch_cmd,bool block) {
	return [watch_cmd, block]() {
		execute_program_args(watch_cmd, false, block);
		};
}

void paste(bool newline, bool isUTF8) {
	std::string clipboard{};


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

void paset_watch(std::string watch_cmd, bool block) {

	exit(
		create_watch(clipboard_update_action(watch_cmd, block))
	);
}


int main(int argc, char* argv[]) {

	parser parser(argc, argv);


	if (parser.help_opt) {
		parser.help();
		exit(0);
	}

	if (parser.sub_command == "copy") copy(parser.msg_opt, parser.UTF8IO_opt);
	else if (parser.sub_command == "paste") {
		if(parser.watch_cmd == "")
			paste(parser.newline_opt, parser.UTF8IO_opt);
		else {
			paset_watch(parser.watch_cmd, parser.block_opt);
		}
	}
	parser.help();
	return 0;
}