Custom Shell

This project is a simple command-line shell implemented in C. It supports executing commands, managing aliases, changing directories, and running script files.

Features

Execute standard shell commands.

Define and use aliases.

Change directories (cd command).

Read and execute script files (source command).

Track command, alias, and script counts dynamically.

Installation and Setup

Prerequisites

A C compiler (e.g., gcc)

A Unix-based OS (Linux/macOS) recommended

Compilation

To compile the shell, run:

gcc -o customShell customShell.c

Running the Shell

To start the shell, use:

./customShell

Usage

Run commands as you would in a regular shell.

Use alias name="command" to create an alias.

Use unalias name to remove an alias.

Use cd <directory> to change directories.

Use source <file> to execute a script file.

Type exit_shell to exit.

Ensure that the necessary permissions and paths are set up correctly before execution.

Contributing

Feel free to fork this repository and submit pull requests for improvements.
