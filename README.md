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
    myuptime <command>: Uptime command shows how long the system has been running since the last reboot and time now
    myfree <command>: provides information about memory usage on the system.
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




-example of output:
        pwd
        Token[0]: 'pwd'
        number of tokens:1
        /home/vboxuser/Desktop/assem/shell
        Child exited with status 0
        ls
        Token[0]: 'ls'
        number of tokens:1
         a.out	      commands.h   shell.out     shell.zip
         commands.c   commands.exec	   shell.c     
        Child exited with status 0
        mv shell.c /home/vboxuser/Desktop
        Token[0]: 'mv'
        Token[1]: 'shell.c'
        Token[2]: '/home/vboxuser/Desktop'
        number of tokens:3
        Child exited with status 0  
        myfree
        Token[0]: 'myfree' 
        number of tokens: 1
                      total        used        free      shared  buff/cache   available
        Mem:        8122592 	 5570076    2552516        0     3474400      5857600
        Swap:       2097148        0 	    2097148
        myuptime
        Token[0]: 'myuptime' 
        number of tokens: 1
        Current time: 2024-08-07 17:29:54
        Uptime: 0 days, 0 hours, 53 minutes, 26 seconds
        ls | grep file
        Token[0]: 'ls' 
        Token[1]: '|' 
        Token[2]: 'grep' 
        Token[3]: 'file' 
        number of tokens: 4
        Command1: 'ls'
        Command2: 'grep file'
        file.c
        file.out

-also shell supports:
1-Piping (|)
    Concept: Pipes are used to pass the output of one command as input to another command.
    
How It Works: 
    When a pipe (|) is detected in the command line, the shell splits the input into two separate commands.
    These commands are then executed in sequence, with the output of the first command being fed directly into the second command.
    as :command1 | command2

2-Redirection (<, >, 2>)
Concept: Redirection changes where the input comes from or where the output goes.

Types of Redirection:

Input Redirection (<): Takes input from a file instead of standard input.
Output Redirection (>): Sends output to a file instead of standard output.
Error Redirection (2>): Redirects error messages to a file instead of standard error.

as:
    command < inputfile
    command > outputfile
    command 2> errorfile



(I have added many debugging lines to be able to notice every thing happens and result as determining each token you have entered and if error happens determine from where )
