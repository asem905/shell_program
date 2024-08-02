This repository contains a simple shell implementation in C that supports basic command functionalities. It includes implementations for common shell commands and demonstrates how to handle user input, parse commands, and execute them.
Overview

This project implements a basic shell with support for built_in_commands like mypwd, myecho, mycp, mymv, myexit, myrm, myhelp, envir, mycd, and mytype. The shell reads commands from standard input, parses them, and executes them accordingly also external commands of any shell as ls,pwd,cat,cp,mv,...
Commands

    mypwd: Print the current working directory.
    myecho [args...]: Print arguments to the terminal.
    mycp [-a] <source> <destination>: Copy files from source to destination, with optional append mode.
    mymv [-f] <source> <destination>: Move files from source to destination, with optional force mode.
    myexit: Exit the shell.
    myrm <file>: Remove the specified file.
    myhelp [command]: Display help information about a specific command.
    envir: Print environment variables.
    mycd <path>: Change the current directory to the specified path.
    mytype <command>: Determine if a command is internal or external.

Libraries and Headers Used

    unistd.h: Provides access to the POSIX operating system API.
    string.h: Provides string handling functions.
    stdio.h: Provides input and output functions.
    stdlib.h: Provides memory allocation and process control functions.
    errno.h: Provides error codes.
    limits.h: Defines constants for system limits.
    fcntl.h: Provides file control options.
    libgen.h: Provides functions for file name manipulation (e.g., basename).
    dirent.h: Provides directory entry functions.
    sys/types.h: Defines data types used in system calls.
    sys/wait.h: Provides macros related to process termination.
    sys/stat.h: Provides file status functions.


-to compile it:    gcc shell.c commands.c commands.h -o shell.out
-to run it:        ./shell.out
