#include <windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <cstddef>
#include <functional>
#include "common.h"


void copy(std::string& arg_msg, bool isUTF8IN) {
	std::string clipboard_data;
	if (arg_msg.empty()) {
		std::ostringstream oss{};
		oss << std::cin.rdbuf();
		clipboard_data = oss.str();
	}
	else {
		clipboard_data = arg_msg;
	}
	int ret;
	if (isUTF8IN) {
		auto wmsg = convert_str_to_wstr(clipboard_data);
		ret = copy_data_to_clipboard(wmsg);
	}
	else {
		ret = copy_data_to_clipboard(clipboard_data);

	}
	exit(ret);
}


std::function<void()> clipboard_update_action(std::string watch_cmd, bool block, bool isUTF8, bool newline) {
	return [watch_cmd, block, isUTF8, newline]() {
		std::string clipboard{};
		int ret = paste_from_clipboard(clipboard, isUTF8);
		if (ret) {
			return;
		}
		if (newline) {
			clipboard += "\n";
		}
		if (watch_cmd.empty()) {
			ret = write_raw_data_to_stdout(clipboard);
			if (ret) {
				exit(ret);
			}
			return;
		}
		if (execute_program_args(watch_cmd, block, clipboard)) {
			print_error("Execute command failed.", false);
			return;
		}
		};
}

void paste(bool newline, bool isUTF8) {
	std::string clipboard{};
	int ret = paste_from_clipboard(clipboard, isUTF8);

	if (ret) {
		exit(ret);
	}

	if (newline) {
		clipboard += "\n";
	}
	ret = write_raw_data_to_stdout(clipboard);
	if (ret) {
		exit(ret);
	}

	exit(0);
}

void paste_watch(std::string watch_cmd, bool wait, bool isUTF8, bool newline) {

	exit(
		create_watch(clipboard_update_action(watch_cmd, wait, isUTF8, newline))
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
		if (parser.watch_opt)
			paste_watch(parser.watch_cmd, parser.block_opt, parser.UTF8IO_opt, parser.newline_opt);
		else {
			paste(parser.newline_opt, parser.UTF8IO_opt);
		}
	}
	else {
		parser.help();
	}
	return 0;
}