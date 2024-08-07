/*
Author:Assem Samy
File:commands.h
comment:contains all functions declarartions used in shell.c
*/

#ifndef COMMANDS_H
#define COMMANDS_H
/*-----------------includes section--------------*/
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
#include <sys/sysinfo.h>
#include <time.h>

/*----------------macros declarations-----------------*/
#define STDIN 0
#define STDOUT 1
#define STDERR 2
#define BUF_SIZE 4096
#define TOKEN_LENGTH 30
#define MAX_TOKEN_LENGTH 100
#define MAX_PATH_LENGTH 1024
#define MAX_ARGS 100
#define BUF_SIZE_MEM 256
#define MAX_LINE 1024
#define MAX_TOKENS 50
#define MAX_CMD_LENGTH 1024
#define MAX_ARG_LENGTH 100
#define MAX_ARGS 100

/*-------------------extern variables------------------*/
extern char **environ;
/*-----------section contains functions declarations used inshell.c file------------*/
void copy_file(const char *source, const char *destination, int append);
void move_file(const char *source, const char *destination_dir,int force);
void remove_file(const char *path) ;
void env_command();
void cd_command(const char *path);
char is_internal(const char *command,const char **commands_given);
int is_external(char *command);
void execute_command(char command[][50],int count);
void echo_command(int arg_count, char args[][50]);
void free_command();
void get_uptime_with_time();
void pipe_commands(char *cmd1, char *cmd2);
#endif
