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
	watch_cmd{""},
	sub_command{ "" },
	help_opt(false),
	UTF8IO_opt(false), 
	newline_opt(false),
	block_opt(false)
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
	sub_command = args.size() == 1? "": args[1];

	// options
	for (auto arg = args.begin() + 2; arg < args.end(); ++arg) {
		if (*arg == "-h") help_opt = true;
		else if (*arg == "-u") UTF8IO_opt = true;
		else if (*arg == "-n") newline_opt = true;
		else if (*arg == "-b") block_opt = true;
		else if (*arg == "-m") {
			msg_opt = std::reduce(arg + 1, args.end(), std::string{},
				[](auto acc, auto str) {
					if(acc != "") acc.append(" ");
					acc.append(str);
					return acc;
				});
			break;
		}
		else if (*arg == "-w") {
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
	std::cout << "win-clip (https://github.com/uclipboard/win-clip): A part of uclipboard." << std::endl;
	std::cout << "Usage: win-clip [SUBCMD] [OPTION] ..." << std::endl;
	std::cout << "sub-cmds: copy/paste" << std::endl;
	std::cout << "options:" << std::endl;
	std::cout << "-h\t display this help and exit." << std::endl;
	std::cout << "-n\t print message from system clipboard with newline in `paste` mode." << std::endl;
	std::cout << "-m\t pass message you want to copy to system clipboard in `copy` mode." << std::endl;
	std::cout << "-u\t print or receive text will be treated as UTF-8 encoded. " << std::endl;
	std::cout << "\t read UTF-8 encoding text from stdin and convert to ANSI (and copy to clipboard).\n\tread ANSI encoded text from clipboard, convert to UTF-8 encoding and print it to stdout." << std::endl;
	std::cout << "-w\t execute the command after this arguments when system clipboard changed in `paste` mode." << std::endl;
	std::cout << "-b\t blocking and waiting the watch command to stop in `paste watch` mode." << std::endl;
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


}
