# win-clip

[![Build Status](https://github.com/dangjinghao/win-clip/workflows/Build/badge.svg)](https://github.com/dangjinghao/win-clip/actions)
[![Release](https://github.com/dangjinghao/win-clip/workflows/Release/badge.svg)](https://github.com/dangjinghao/win-clip/releases)

A command-line clipboard utility for Windows, part of the `uclipboard` project.

**To get started, download the latest binary from the releases page.**

## Features

- **UTF-8 Support**: Ensures seamless cross-platform text transfer by handling UTF-8 encoding conversions automatically.
- **Watch Mode**: Monitors the clipboard for changes and can:
  - Execute a command, passing the new clipboard content to its `stdin`.
  - Print the new content to `stdout` if no command is specified.

## Usage Notes

When copying text from the command line, arguments are concatenated with a space.

```shell
# Copies "hello world" to the clipboard
win-clip copy hello world
```

To copy text containing multiple spaces or special characters, enclose it in quotes.

```shell
# Copies "some data with  extra spaces"
win-clip copy "some data with  extra spaces"
```

For a complete list of options, see the help menu:

```shell
win-clip.exe -h
```
