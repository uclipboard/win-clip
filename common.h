#pragma once
#include <Windows.h>
#include <string>
#include <vector>
#include <functional>
#include <iostream>
#include <format>

int copy_data_to_clipboard(std::string& msg);
int copy_data_to_clipboard(std::wstring& wmsg);

int get_clipboard_content(std::string& s);
int get_clipboard_content(std::wstring& ws);

int paste_from_clipboard(std::string& s, bool isUTF8);
int create_watch(std::function<void()> func_callback);
std::wstring convert_str_to_wstr(const std::string& utf8string, const int CODE_PAGE = CP_UTF8);
std::string convert_wstr_to_str(std::wstring& wstr, const int code_page = CP_ACP);
int execute_program_args(std::string command, bool wait, std::string stdin_data);
int write_raw_data_to_stdout(std::string& s);

class parser {
public:
	std::vector<std::string> args;
	std::string prog_name;
	std::string sub_command;
	std::string msg_opt;
	std::string watch_cmd;
	bool newline_opt;
	bool UTF8IO_opt;
	bool help_opt;
	bool block_opt;
	parser(int argc, char* argv[]);
	void help();

};

const static int ERRCODE_UNKNOWN = -1;
const static int ERRCODE_CLIPBOARD_EMPTY = -2;
const static int ERRCODE_CLIPBOARD_DATA_TYPE_UNKNOWN = -3;
const static int ERRCODE_CONVERT_WIDE_SIZE_FAILED = -4;



static inline void print_error(std::string s, bool get_last_error = true) {
	std::cerr << std::format("[{}]", get_last_error ? GetLastError() : ERRCODE_UNKNOWN) << s << std::endl; 
}

static inline void print_error(std::string s, int error_code) {
	std::cerr << std::format("[{}]", error_code) << s << std::endl;
}