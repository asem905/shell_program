# Simple Shell Implementation in C

## Overview
This repository contains a simple shell implementation in C that supports basic command functionalities. It demonstrates how to handle user input, parse commands, and execute them. The shell includes implementations for both built-in commands and external shell commands.

## Supported Commands
The shell supports several built-in commands, as well as external commands such as `ls`, `pwd`, `cat`, `cp`, `mv`,`ps`,`bash`,`grep` and more.

### Commands made
- **mypwd**: Prints the current working directory.
- **myecho [args...]**: Prints the provided arguments to the terminal.
- **mycp [-a] <source> <destination>**: Copies files from the source to the destination, with an optional append mode.
- **mymv [-f] <source> <destination>**: Moves files from the source to the destination, with an optional force mode.
- **myexit**: Exits the shell.
- **myrm <file>**: Removes the specified file.
- **myhelp [command]**: Displays help information for a specific command.
- **envir**: Prints environment variables.
- **mycd <path>**: Changes the current directory to the specified path.
- **mytype <command>**: Determines whether a command is internal or external.
- **myuptime**: Shows how long the system has been running since the last reboot and displays the current time.
- **myfree**: Provides information about memory usage on the system.
- **myls**: This command allows the use of multiple options in any combination or order. Supported options include `a`, `l`, `d`, `t`, `i`, `u`, `c`, `1`, and `f`. You can mix and match these options freely to customize the output to your needs.
-**|**:The pipe operator allows you to connect the output of one command directly to the input of another. This enables you to create powerful command combinations, processing data in a streamlined manner. You can chain multiple commands together, using the pipe to pass the results from one to the next, enhancing the overall functionality and efficiency of your shell commands.
-**<, >, 2>**: Redirection operators allow you to control where input comes from and where output goes.
  
## Libraries and Headers Used
The shell relies on the following libraries and headers:
- `unistd.h`: POSIX operating system API.
- `string.h`: String handling functions.
- `stdio.h`: Input and output functions.
- `stdlib.h`: Memory allocation and process control functions.
- `errno.h`: Error codes.
- `limits.h`: System limits constants.
- `fcntl.h`: File control options.
- `libgen.h`: Functions for file name manipulation (e.g., `basename`).
- `dirent.h`: Directory entry functions.
- `sys/types.h`: Data types for system calls.
- `sys/wait.h`: Macros for process termination.
- `sys/stat.h`: File status functions.

## Compilation and Execution
To compile the shell:
```bash
gcc shell.c commands.c commands.h -o shell.out
```

To run the shell:
```bash
./shell.out
```

## Example of Output
```bash
pwd
Token[0]: 'pwd'
Number of tokens: 1
/home/user/Desktop/shell
Child exited with status 0

ls
Token[0]: 'ls'
Number of tokens: 1
a.out  commands.h  shell.out  shell.zip  commands.c  shell.c
Child exited with status 0

mv shell.c /home/user/Desktop
Token[0]: 'mv'
Token[1]: 'shell.c'
Token[2]: '/home/user/Desktop'
Number of tokens: 3
Child exited with status 0  

myfree
Token[0]: 'myfree' 
Number of tokens: 1
              total       used       free     shared  buff/cache   available
Mem:        8122592    5570076    2552516        0     3474400     5857600
Swap:       2097148        0      2097148

myuptime
Token[0]: 'myuptime' 
Number of tokens: 1
Current time: 2024-08-07 17:29:54
Uptime: 0 days, 0 hours, 53 minutes, 26 seconds

ls | grep file
Token[0]: 'ls' 
Token[1]: '|' 
Token[2]: 'grep' 
Token[3]: 'file' 
Number of tokens: 4
Command1: 'ls'
Command2: 'grep file'
file.c
file.out

myls -flut
Token[0]: 'myls' 
Token[1]: '-flut' 
number of tokens: 2
-rw-rw-r--   1    vboxuser vboxuser 8851     2024-09-20 15:15     commands (copy).c             
drwxrwxr-x   4    vboxuser vboxuser 4096     2024-09-20 15:13     .                             
-rw-rw-r--   1    vboxuser vboxuser 58656    2024-09-20 15:13     commands.c                    
-rwxrwxr-x   1    vboxuser vboxuser 69696    2024-09-20 15:13     shell2.out                    
drwxrwxr-x   4    vboxuser vboxuser 4096     2024-09-20 15:10     session10                     
drwxrwxr-x   2    vboxuser vboxuser 4096     2024-09-20 15:10     urg                           
drwxrwxr-x   9    vboxuser vboxuser 4096     2024-09-20 15:10     ..                            
-rw-rw-r--   1    vboxuser vboxuser 2436     2024-09-19 18:57     commands.h                    
-rw-rw-r--   1    vboxuser vboxuser 14902    2024-09-19 18:41     shell.c                       
-rw-rw-r--   1    vboxuser vboxuser 4961     2024-09-19 17:47     file.c                        
-rw-rw-r--   1    vboxuser vboxuser 533      2024-09-19 17:40     uidtouname.c                  
-rw-r--r--   1    vboxuser vboxuser 2156     2024-09-19 15:33     t_getopt.c                    
-rw-rw-r--   1    vboxuser vboxuser 830      2024-09-19 13:46     lsv1.c                        
-rw-rw-r--   1    vboxuser vboxuser 1550     2024-09-16 17:06     pipes.c                       
-rw-rw-r--   1    vboxuser vboxuser 709      2024-09-16 16:28     qsort.c                       
-rw-rw-r--   1    vboxuser vboxuser 782      2024-09-16 15:21     lsv0.c                        
-rwxrwxr-x   1    vboxuser vboxuser 16376    2024-09-15 17:43     opt.out                       
-rw-rw-r--   1    vboxuser vboxuser 1070     2024-09-15 16:12     filetype.c                    
-rw-rw-r--   1    vboxuser vboxuser 741      2024-09-15 16:12     fileinfo.c                    
-rw-rw-r--   1    vboxuser vboxuser 1125     2024-09-15 16:12     filepermissions.c             
-rw-rw-r--   1    vboxuser vboxuser 535      2024-09-15 16:12     gidtogname.c                  
-rwxr-xr-x   1    vboxuser vboxuser 305      2024-09-15 16:12     myargs.c                      
-rw-rw-r--   1    vboxuser vboxuser 321      2024-09-15 16:12     transformtime.c               
-rwxrwxr-x   1    vboxuser vboxuser 16264    2024-09-12 14:13     sort.out                      
-rw-rw-r--   1    vboxuser vboxuser 927      2024-09-12 14:08     lsv2.c                        
-rwxrwxr-x   1    vboxuser vboxuser 16368    2024-09-11 18:10     ls0.out                       
-rwxrwxr-x   1    vboxuser vboxuser 17632    2024-08-13 18:56     try.out                       
-rw-rw-r--   1    vboxuser vboxuser 307      2024-08-13 18:56     try.c                         
-rwxrwxr-x   1    vboxuser vboxuser 15960    2024-08-12 22:57     try                           
-rw-rw-r--   1    vboxuser vboxuser 7785     2024-08-07 16:52     shell.zip                     
-rw-rw-r--   1    vboxuser vboxuser 0        2024-08-06 21:36     test.c                        
-rwxrwxr-x   1    vboxuser vboxuser 16520    2024-08-06 19:45     pipe.out                      
-rw-r--r--   1    vboxuser vboxuser 202      2024-08-05 23:58     out                           
-rwxrwxr-x   1    vboxuser vboxuser 16336    2024-08-05 23:58     file.out                      
-rwx------   1    vboxuser vboxuser 0        2024-08-05 23:57     output                        
-rw-rw-r--   1    vboxuser vboxuser 0        2024-08-05 23:53     assem                         
-rw-rw-r--   1    vboxuser vboxuser 2        2024-08-04 16:55     input                         
-rwxrwxr-x   1    vboxuser vboxuser 21864    2024-08-02 22:50     shell.out                     
-rwxrwxr-x   1    vboxuser vboxuser 21536    2024-07-30 21:26     commands.exec
```

## Additional Features
### 1. Piping (`|`)
**Concept**: Pipes are used to pass the output of one command as input to another command.
- **How It Works**: When a pipe (`|`) is detected, the shell splits the input into two commands. The output of the first command is used as the input to the second command.
  Example: `command1 | command2`

### 2. Redirection (`<`, `>`, `2>`)
**Concept**: Redirection changes the source of input or the destination of output.
- **Types of Redirection**:
  - **Input Redirection (`<`)**: Reads input from a file instead of standard input.
  - **Output Redirection (`>`)**: Writes output to a file instead of standard output.
  - **Error Redirection (`2>`)**: Redirects error messages to a file instead of standard error.

Examples:
- `command < inputfile`
- `command > outputfile`
- `command 2> errorfile`

## Debugging
Numerous debugging lines have been added to the shell to track user input, token parsing, and error detection. This makes it easier to trace what happens during execution and to locate errors. For example, after each command is entered, the shell reports the number of tokens and their values, as well as whether the command executed successfully.

--- 

# `ls_command_implementation`

This section describes a custom implementation of the `ls` command for listing directory contents. It supports various options to control file display, sorting, and additional functionalities like displaying inodes or sorting by time.

## Features
- **Show Hidden Files**: Display hidden files (files starting with `.`) using the `-a` option.
- **Long Format Listing**: Use the `-l` option to display detailed information (permissions, ownership, size, modification date).
- **Inode Display**: Display inode numbers with the `-i` option.
- **Time-Based Sorting**: Sort files by modification time (`-t`), access time (`-u`), or status change time (`-c`).
- **No Sorting**: Use `-f` to disable sorting.
  
## Usage
```bash
ls_command_implementation [options] [directory...]
```

### Options:
- `-a`: Show hidden files.
- `-l`: Use long listing format.
- `-d`: Display directory names instead of their contents.
- `-t`: Sort by modification time.
- `-i`: Show inode numbers.
- `-u`: Sort by access time.
- `-c`: Sort by status change time.
- `-1`: Show one file per line.
- `-f`: Disable sorting.

---


### Variables and Structures

- **FileEntry Struct**  
  This structure stores file metadata for sorting and display purposes.
  ```c
  typedef struct {
      char name[TOKEN_LENGTH]; // Name of the file
      time_t time;             // Stores time related to the file (can be change, modification, or access time)
      int type;                // Type of time (0 for change, 1 for modification, 2 for access time)
      int sort_type;           // Sorting criteria (0 for time, 1 for name)
  } FileEntry;
  ```

### Static Functions

1. **compare_time**  
   Compares two `FileEntry` structs by their `time` field, sorting in descending order.
   ```c
   static int compare_time(const void *a, const void *b);
   ```

2. **usageError**  
   Prints a usage error message and exits the program.
   ```c
   static void usageError(const char *progName, const char *msg, int opt);
   ```

3. **compare_name**  
   Compares two `FileEntry` structs by their `name` field for sorting.
   ```c
   static int compare_name(const void *p1, const void *p2);
   ```

4. **is_directory**  
   Checks if a given path is a directory.
   ```c
   static int is_directory(const char *path);
   ```

5. **do_ls_directory**  
   Lists the contents of a directory, optionally hiding files, sorting them, and displaying them in a line-by-line or column format.
   ```c
   static void do_ls_directory(char *dir, int hidden_files, int sorting_required, int line_by_line);
   ```

6. **show_stat_info_each**  
   Displays detailed file information, including permissions, owner, group, size, and modification times, while supporting sorting and hidden files.
   ```c
   static void show_stat_info_each(char token[][TOKEN_LENGTH], const char *dir, char t_check, int sort_by_name, int hidden_files, int inode_print);
   ```

7. **show_directory**  
   Prints a formatted listing of directory contents, including file permissions, number of links, user, group, size, and modification time.
   ```c
   static void show_directory(char token[][TOKEN_LENGTH], char *dir, int l_check, int inode_print);
   ```

8. **show_inode_number**  
   Displays the inode number of files and directories, with options for line-by-line display and sorting.
   ```c
   static void show_inode_number(char token[][TOKEN_LENGTH], char *dir, int inode_long_format_check, int line_by_line, int sorting_required);
   ```

9. **sort_files**  
   Sorts files by name or time (change, modification, or access time), and optionally hides files.
   ```c
   static void sort_files(char token[][TOKEN_LENGTH], const char *dir, int time_type, int line_by_line, int hidden_files);
   ```

---


## Debugging:
Numerous debugging lines have been added to the shell to track user input, token parsing, and error detection, making it easier to identify issues during execution.

