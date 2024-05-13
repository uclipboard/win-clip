# win-clip
This is a part of uclipboard project

# Usage
Usage: win-clip [MODE] [OPTION] ...
options:
-h       display this help and exit.
-n       print message from system clipboard with newline in `paste` mode.
-m       pass message you want to copy to system clipboard in `copy` mode.
-u       print or receive content will be treated as UTF8 encoded if this flag was specified in `paste` mode.

example:
win-clip -h      display help.
win-clip copy -m hello world     copy 'hello world' to system clipboard.
win-clip copy    copy the message readed from stdin to system clipboard.
or: echo hello world | win-clip copy     copy hello world to system clipboard.

win-clip paste   print the latest message from system clipboard.
win-clip paste -n        print the latest message from system clipboard with newline.
By the way, win-clip supports alias name, so you can hard or soft link to to some special name and use it in this way:

> mklink.exe /H win-paste.exe win-clip.exe
> mklink.exe /H win-copy.exe win-clip.exe

win-paste == win-clip paste and win-copy == win-clip copy
echo hello world | win-copy
win-copy -m hello world
win-paste [-h]
## Features
- 

## Bugs or Features
win-clip supports `win-clip copy -m hello world` to copy `hello wolrd` to system clipboard. But in fact, it operate on an approach of concatenate text with whitespace as the separate . So If you want to copy some data contains no-whitespace invisible character, please use quotation. 
