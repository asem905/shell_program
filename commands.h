/*
Author:Assem Samy
File:commands.h
comment:contains all functions declarartions used in shell.c
*/

#ifndef COMMANDS_H
#define COMMANDS_H
/-----------------includes section--------------/
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include <fcntl.h>
#include <libgen.h> // Include for basename()
#include <dirent.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/sysinfo.h>
#include <time.h>
#include <dirent.h>
#include <ctype.h>
#include <getopt.h>
#include <grp.h>
#include <pwd.h>
#include <locale.h>
/----------------macros declarations-----------------/
#define INITIAL_CAPACITY 1000
#define STDIN 0
#define STDOUT 1
#define STDERR 2
#define BUF_SIZE 4096
#define TOKEN_LENGTH 50
#define MAX_TOKEN_LENGTH 100
#define MAX_PATH_LENGTH 1024
#define MAX_ARGS 100
#define BUF_SIZE_MEM 256
#define MAX_LINE 1024
#define MAX_TOKENS 50
#define MAX_CMD_LENGTH 1024
#define MAX_ARG_LENGTH 100
#define C_TIME_SORT			 0
#define MODIFICATION_TIME_SORT		 1
#define ACCESS_TIME_SORT		 2
#define MAX_ARGS 100
#define MODIFICATION_TIME  		'm'
#define C_TIME		   		'c'
#define ACCESS_TIME        		'a'
#define INODE_LONG_FORMAT   	 	 1
#define NO_INODE_LONG_FORMAT     	 1
#define LONG_FORMAT_CHECK_WITH_INODE	 1
#define NOLONG_FORMAT_CHECK_WITH_INODE	 0
#define SORT_BY_NAME			 1
#define NOSORT_BY_NAME			 0
#define NOSORT_BY_TIME			 -1
#define SORTING_REQUIRED		 1
#define NOSORTING_REQUIRED		 0
#define LINE_BYLINE_PRINT		 1
#define NOLINE_BYLINE_PRINT		 0
#define COLUMN_WIDTH 			 20
#define MAX_ENTRIES 			 1000
/-------------------extern variables------------------/
extern char **environ;
/-----------section contains functions declarations used inshell.c file------------/
void copy_file(const char *source, const char *destination, int append);
void move_file(const char *source, const char *destination_dir,int force);
void remove_file(const char *path) ;
void env_command();
void cd_command(const char *path);
char is_internal(const char *command,const char **commands_given);
int is_external(char *command);
void execute_command(char command[][TOKEN_LENGTH],int count);
void echo_command(int arg_count, char args[][50]);
void free_command();
void get_uptime_with_time();
void pipe_commands(char *cmd1, char *cmd2);
void ls_command_implementation(int argc, char token[][TOKEN_LENGTH]) ; 
#endif
