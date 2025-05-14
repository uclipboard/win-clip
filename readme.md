# win-clip

This is a part of uclipboard project.

**How to use: download it from release page.**

## Features
- Support UTF-8 IO (paste/print ANSI encoded text after UTF-8 encoding converted or copy UTF-8 encoding data but converted to ANSI when copying into clipboard). This feature is very useful when you transfer the clipboard text across platforms.
- Watch mode: execute a command and pass clipboard content to its stdin as soon as clipboard changed. Or just print clipboard content to stdout if the command is empty.
## Bugs or Features
win-clip supports `win-clip copy -m hello world` to copy `hello wolrd` to system clipboard.
But in fact, it operate on an approach of concatenate text with whitespace as the separate.
So If you want to copy some data contains no-whitespace invisible character, please use quotation. 

See `win-clip.exe -h` for more details.