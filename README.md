# C++ Shell

A lightweight Unix-style command-line shell built from scratch using **C++17**.

This project explores the fundamentals of operating systems and shell development, including command parsing, process creation, executable resolution, and file descriptor management.

The goal is to gradually build a functional Unix-style shell while gaining a deeper understanding of how command interpreters work internally.

## Features

### Currently Implemented

**Built-in Commands**
- `exit` — Exit the shell.
- `echo` — Print arguments to standard output.
- `type` — Identify shell built-ins or locate executables in PATH.
- `pwd` — Display the current working directory.
- `cd` — Change the working directory, including support for home directory expansion (`~`).

**Command Execution**
- Execute external programs found in the system PATH.
- Create child processes using `fork()`.
- Execute programs using `execv()`.
- Wait for child processes using `waitpid()`.

**Command Parsing**
- Whitespace-separated arguments.
- Single-quoted strings.
- Double-quoted strings.
- Basic backslash escape handling.
- Preservation of spaces within quoted arguments.

**Output Redirection**
- Redirect standard output using `>` and `1>`.
- Supports redirection for both built-in commands and external programs.
- Uses `open()` and `dup2()` for file descriptor manipulation.
- Existing output files are overwritten.

## Getting Started

### Prerequisites

- Linux or another compatible POSIX environment
- A C++17-compatible compiler (GCC or Clang)

### Clone the Repository

```bash
git clone https://github.com/<username>/cpp-shell.git
cd cpp-shell
```

Replace `<username>` with your GitHub username.

### Build

Compile directly using G++:

```bash
g++ -std=c++17 -Wall -Wextra -Wpedantic main.cpp -o cpp-shell
```

### Run

```bash
./cpp-shell
```

## Usage Examples

**Built-in commands**

```text
$ echo "Hello, World!"
Hello, World!

$ pwd
/home/user

$ type echo
echo is a shell builtin

$ type ls
ls is /usr/bin/ls
```

**Working with directories**

```text
$ cd /tmp
$ pwd
/tmp

$ cd ~
```

**Executing external commands**

```text
$ ls
$ date
$ uname -a
```

**Output redirection**

```text
$ echo "Hello, Shell!" > output.txt
$ cat output.txt
Hello, Shell!

$ pwd 1> current_directory.txt
```

## How It Works

The shell follows a basic **Read–Parse–Execute** cycle:

1. **Read:** Accept a command from standard input.
2. **Parse:** Split the input into arguments while handling quotes and escaping.
3. **Process Redirection:** Detect supported output redirection operators.
4. **Resolve:** Determine whether the command is a built-in or an external executable.
5. **Execute:** Run built-ins directly or use `fork()` and `execv()` for external programs.
6. **Repeat:** Continue accepting commands until `exit` or end-of-file.

### Core System Calls

| Function | Purpose |
|---|---|
| `fork()` | Create a child process |
| `execv()` | Execute an external program |
| `waitpid()` | Wait for child process completion |
| `access()` | Check executable accessibility |
| `open()` | Open files for redirection |
| `dup()` / `dup2()` | Save and redirect file descriptors |
| `close()` | Close file descriptors |

## Project Roadmap

### Phase 1 — Shell Fundamentals
- [x] Interactive command prompt
- [x] Basic command parsing
- [x] Built-in commands
- [x] PATH resolution
- [x] External command execution
- [x] Single and double quotes
- [x] Basic escape sequences
- [x] Standard output redirection

### Phase 2 — Redirection and Parser Improvements
- [ ] Append redirection (`>>`)
- [ ] Standard error redirection (`2>`, `2>>`)
- [ ] Input redirection (`<`)
- [ ] Improved parsing and error handling
- [ ] Direct executable paths (`./program`)
- [ ] Automated tests

### Phase 3 — Pipelines and Shell Operators
- [ ] Command pipelines (`|`)
- [ ] Sequential execution (`;`)
- [ ] Conditional execution (`&&`, `||`)
- [ ] Environment variable expansion
- [ ] Exit status tracking (`$?`)

### Phase 4 — Interactive Shell Improvements
- [ ] Command history
- [ ] Arrow-key navigation
- [ ] Tab completion
- [ ] Signal handling
- [ ] Background processes (`&`)
- [ ] Basic job control

## Current Limitations

This project is under active development and is not intended to replace Bash, Zsh, or other production shells.

Currently:
- Pipelines and command chaining are not supported.
- Input and stderr redirection are not implemented.
- Environment variable expansion is not available.
- Redirection operators must be separated from adjacent arguments by whitespace.
- Direct executable paths such as `./program` are not yet supported.
- Command history and job control are not implemented.

## Project Goals

Through this project, I aim to improve my understanding of:

- C++ and modern programming practices
- Linux and POSIX system programming
- Process creation and management
- File descriptors and I/O redirection
- Command-line parsing
- Operating system fundamentals
- Modular software architecture

## Development Status

**Work in Progress**

The shell is being developed incrementally, with an emphasis on understanding and implementing core functionality before introducing more advanced features.