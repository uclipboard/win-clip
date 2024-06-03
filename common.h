#pragma once
#include <Windows.h>
#include <string>
#include <vector>
#include <functional>
int copy_UTF8_to_clipboard(std::string& msg);
int copy_ANSI_to_clipboard(std::string& msg);
int get_clipboard_content(std::string& s);
int get_clipboard_content(HANDLE& hData, std::wstring& s);
int paste_from_clipboard(std::string& s, bool isUTF8);
int create_watch(std::function<void()> func_callback);
std::wstring convert_str_to_wstr(const std::string& utf8string, const int CODE_PAGE = CP_UTF8);
std::string convert_wstr_to_str(std::wstring& wstr, const int code_page = CP_ACP);
int execute_program_args(std::string command, bool UTF8_in, bool wait = false);


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


#define LAST_ERROR_CODE "("<<GetLastError()<<")"
