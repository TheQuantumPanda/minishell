# C++ Shell

A lightweight Unix-style command-line shell built using **C++17**.

This project is part of my journey through the [CodeCrafters](https://codecrafters.io/) *Build Your Own Shell* challenge, where I'm learning how shells work by implementing one myself.

It explores operating system fundamentals, including command parsing, process creation, executable resolution, and file descriptor management.

The goal is to gradually build a functional Unix-style shell while gaining a deeper understanding of what happens behind the terminal.

## Features

### Currently Implemented

**Built-in Commands**
- `exit` — Exit the shell.
- `echo` — Print arguments to standard output.
- `type` — Identify shell built-ins or locate executables in PATH.
- `pwd` — Display the current working directory.
- `cd` — Change the working directory, including basic home directory expansion (`~`).

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
- Redirection operators must be separate arguments, with spaces around them.

## Getting Started

### Prerequisites

- Linux or another compatible POSIX environment
- A C++17-compatible compiler (GCC or Clang)
- Git (for cloning the repository)

### Clone the Repository

```bash
git clone https://github.com/TheQuantumPanda/minishell.git
cd minishell
```

### Build

Compile directly using G++:

```bash
g++ -std=c++17 -Wall -Wextra -Wpedantic main.cpp -o minishell
```

### Run

```bash
./minishell
```

You should now see the shell prompt:

```text
$
```

## Usage Examples

The following examples illustrate supported commands. Directory paths and executable locations may vary depending on your system.

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

**Quoting and escaping**

```text
$ echo "Hello    World"
Hello    World

$ echo 'Hello World'
Hello World

$ echo Hello\ World
Hello World
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

Built-in commands execute within the shell process. For external commands, the shell searches the system PATH, creates a child process, executes the program, and waits for it to finish.

For output redirection, external commands redirect stdout in the child process. Built-in commands use a `RedirectionGuard` to temporarily redirect stdout and restore it afterward.

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

## Known Limitations

This project is still under development and is not intended to replace Bash, Zsh, or other production shells.

Currently:

- Pipelines (`|`) and command chaining (`&&`, `||`, `;`) are not supported.
- Input redirection (`<`), stderr redirection (`2>`), and append redirection (`>>`) are not implemented.
- Only one output redirection is processed per command, and operators must be separated from adjacent arguments by whitespace.
- The parser does not fully distinguish quoted redirection symbols from actual operators or report unmatched quotes.
- Environment variable expansion and wildcard expansion are not supported.
- Direct executable paths such as `./program` are not supported.
- Built-in commands implement basic behavior without all standard shell options.
- Command history, tab completion, and job control are not implemented.

