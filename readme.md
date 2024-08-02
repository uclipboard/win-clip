# win-clip
This is a part of uclipboard project.

**How to use: download it from release**
## Features
- Support UTF-8 IO (paste/print as utf-8 coding or copy data as utf-8 coding to clipboard)
- Watch mode: execute a command and pass clipboard content to its stdin as soon as clipboard changed.
## Bugs or Features
win-clip supports `win-clip copy -m hello world` to copy `hello wolrd` to system clipboard. But in fact, it operate on an approach of concatenate text with whitespace as the separate . So If you want to copy some data contains no-whitespace invisible character, please use quotation. 

See `win-clip.exe -h` for more details.