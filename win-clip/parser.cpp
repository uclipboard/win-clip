#include "common.h"
#include <iostream>
#include <numeric> // for std::reduce
static const std::string copy_suffix{ "win-copy" };
static const std::string paste_suffix{ "win-paste" };
static const std::string ext_execute{ ".exe" };

parser::parser(int argc, char* argv[]) :
	args{},
	prog_name(argv[0]),
	msg_opt{ "" },
	watch_cmd{ "" },
	sub_command{ "" },
	help_opt(false),
	UTF8IO_opt(false),
	newline_opt(false),
	block_opt(false),
	watch_opt(false)
{
	for (int i = 0; i < argc; ++i)
		args.push_back(std::string(argv[i]));

	// ignore ext name
	if (prog_name.ends_with(ext_execute)) {
		prog_name.erase(prog_name.length() - ext_execute.size(), ext_execute.size());
	}

	// alias name
	if (prog_name.ends_with(copy_suffix)) {
		args.insert(args.begin() + 1, "copy");
	}
	else if (prog_name.ends_with(paste_suffix)) {
		args.insert(args.begin() + 1, "paste");
	}
	// sub command
	sub_command = args.size() == 1 ? "" : args[1];
	if (sub_command.empty()) {
		help();
		exit(1);
	}
	// options
	for (auto arg = args.begin() + 2; arg < args.end(); ++arg) {
		if (*arg == "-h") help_opt = true;
		else if (*arg == "-u") UTF8IO_opt = true;
		else if (*arg == "-n") newline_opt = true;
		else if (*arg == "-b") block_opt = true;
		else if (*arg == "-m") {
			msg_opt = std::reduce(arg + 1, args.end(), std::string{},
				[](auto acc, auto str) {
					if (acc != "") acc.append(" ");
					acc.append(str);
					return acc;
				});
			break;
		}
		else if (*arg == "-w") {
			watch_opt = true;
			watch_cmd = std::reduce(arg + 1, args.end(), std::string{},
				[](auto acc, auto str) {
					if (acc != "") acc.append(" ");
					acc.append(str);
					return acc;
				});
			break;
		}
	}

}

void parser::help() {
	std::cout << "win-clip (https://github.com/uclipboard/win-clip): Part of uclipboard." << std::endl;
	std::cout << "Usage: win-clip [SUBCMD] [OPTION] ..." << std::endl;
	std::cout << "Sub-commands: copy / paste" << std::endl;
	std::cout << "Options:" << std::endl;
	std::cout << "-h\t Show this help message and exit." << std::endl;
	std::cout << "-n\t Print the clipboard content with a newline in 'paste' mode" << std::endl;
	std::cout << "-m\t Specify the message to copy to the clipboard in 'copy' mode." << std::endl;
	std::cout << "-u\t Treat input and output as UTF-8 encoded text." << std::endl;
	std::cout << "\t Read UTF-8 text from stdin, convert to ANSI, and copy to clipboard.\n"
		"\t Read ANSI text from clipboard, convert to UTF-8, and print to stdout." << std::endl;
	std::cout << "-w\t Execute the specified command and pass clipboard content to its stdin when the clipboard changes in 'paste' mode.\n"
		"\t If no command is given, print the updated clipboard content to stdout." << std::endl;
	std::cout << "-b\t (deprecated, I don't know what is it used for right now.)Block and wait for the watch command to finish in 'paste watch' mode.\n"
		"\t (Recommended when using '-w win-clip paste' or commands involving mutex actions.)" << std::endl;
	std::cout << std::endl << "Examples:" << std::endl;
	std::cout << "win-clip -h\t Show help." << std::endl;
	std::cout << "win-clip paste -u -w cat.exe\t Run 'cat.exe' when clipboard content updates." << std::endl;
	std::cout << "win-clip copy -m hello world\t Copy 'hello world' to the clipboard." << std::endl;
	std::cout << "win-clip copy\t Copy the message read from stdin to the clipboard." << std::endl;
	std::cout << "echo hello world | win-clip copy\t Copy 'hello world' to the clipboard." << std::endl;
	std::cout << std::endl;
	std::cout << "win-clip paste\t Print the latest clipboard content." << std::endl;
	std::cout << "win-clip paste -n\t Print the latest clipboard content with a newline." << std::endl;
	std::cout << "win-clip paste -w cat.exe\t Pass clipboard content to 'cat.exe' stdin when clipboard changes." << std::endl;
	std::cout << "echo <UTF-8 encoded string> | win-clip copy -u\t Copy ANSI-converted UTF-8 string to clipboard." << std::endl;
	std::cout << "win-clip paste -u\t Print the latest clipboard content as UTF-8 encoded." << std::endl;
	std::cout << "win-clip paste -u -w cat.exe\t Convert clipboard conetent into UTF-8 and pass it to 'cat.exe' stdin when clipboard changes." << std::endl;
	std::cout << std::endl;
	std::cout << "win-clip supports alias names. You can create hard or soft links for special names and use them as follows:" << std::endl;
	std::cout << std::endl;
	std::cout << "> mklink.exe /H win-paste.exe win-clip.exe" << std::endl;
	std::cout << "> mklink.exe /H win-copy.exe win-clip.exe" << std::endl;
	std::cout << std::endl;
	std::cout << "win-paste == win-clip paste, win-copy == win-clip copy" << std::endl;
	std::cout << "echo hello world | win-copy" << std::endl;
	std::cout << "win-copy -m hello world" << std::endl;
	std::cout << "win-paste [-h]" << std::endl;
}
