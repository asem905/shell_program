/*
Author:Assem Samy
File:commands.c
comment:contains all functions definitions used in shell.c
*/
/*-----------------includes section--------------*/
#include "commands.h"


/*-----------section contains functions definitions used inshell.c file------------*/
void copy_file(const char *source, const char *destination, int append) {
    int fsource, fdest;
    ssize_t bytes_read, bytes_written;
    char buffer[BUF_SIZE];

    // Open source file for reading
    fsource = open(source, O_RDONLY);
    if (fsource == -1) {
        perror("opening source failed");
        exit(EXIT_FAILURE);
    }

    // Determine the flags to use for opening destination file
    int open_flags = O_WRONLY | O_CREAT;
    if (append) {
        open_flags |= O_APPEND;
    } else {
        open_flags |= O_TRUNC;
    }

    // Open destination file
    fdest = open(destination, open_flags, 0666);
    if (fdest == -1) {
        perror("opening destination failed");
        exit(EXIT_FAILURE);
    }

    // Copy data from source to destination
    while ((bytes_read = read(fsource, buffer, BUF_SIZE)) > 0) {
        bytes_written = write(fdest, buffer, bytes_read);
        if (bytes_written != bytes_read) {
            perror("writing failed\n");
            exit(EXIT_FAILURE);
        }
    }

    if (bytes_read == -1) {
        perror("reading failed\n");
        exit(EXIT_FAILURE);
    }

    // Close files
    if (close(fsource) == -1) {
        perror("close source failed\n");
        exit(EXIT_FAILURE);
    }

    if (close(fdest) == -1) {
        perror("close destination failed\n");
        exit(EXIT_FAILURE);
    }
}

void move_file(const char *source, const char *destination_dir,int force) {
    char destination[PATH_MAX];

    // Construct the full destination path
    snprintf(destination, sizeof(destination), "%s/%s", destination_dir, basename(source));
    /*snprintf takes where to store result(destination),it's size and then append file with destination directory as basename() is used to extract file name from source*/

    // Check if the destination file already exists
    if (!force && access(destination, F_OK) != -1) {
        fprintf(stderr, "Error: File '%s' already exists in '%s'.\n", basename(source), destination_dir);
        exit(EXIT_FAILURE);
    }

    // Attempt to move the file using rename
    if (rename(source, destination) != 0) {
        perror("rename failed\n");
        exit(EXIT_FAILURE);
    }
    // Attempt to move the file using rename that takes file from source and put it to destination
    
}
void remove_file(const char *path) {
    // Attempt to remove the file using unlink
    /*the unlink function  is used to remove a file from the filesystem*/
    if (unlink(path) != 0) {
        perror("unlink");
        exit(EXIT_FAILURE);
    }
}


void env_command(){
	int i=0;
	while(environ[i]!=NULL){
		printf("%s\n",environ[i]);
		i++;
	}	
}
void cd_command(const char *path) {
    // Check for ".." to go up one directory level
    if (strcmp(path, "..") == 0) {
        char cwd[PATH_MAX];

        // Get the current working directory
        if (getcwd(cwd, sizeof(cwd)) == NULL) {
            perror("getcwd failed");
            exit(EXIT_FAILURE);
        }

        // Find the last slash to truncate the path
        char *last_slash = strrchr(cwd, '/');
        if (last_slash != NULL) {
            *last_slash = '\0';  // Truncate at the last slash
        }

        // Change to the new directory
        if (chdir(cwd) != 0) {
            perror("chdir failed");
            exit(EXIT_FAILURE);
        }
    } else {
        // Change to the specified directory
        if (chdir(path) != 0) {
            perror("chdir failed");
            exit(EXIT_FAILURE);
        }
    }

    // Print the current working directory after change
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("Current working directory: %s\n", cwd);
    } else {
        perror("getcwd failed");
        exit(EXIT_FAILURE);
    }
}

void echo_command(int arg_count, char args[][50]) {
    for (int i = 1; i < arg_count; i++) {
	    if(args!=NULL){
	    	printf("%s ", args[i]);
	    }
    }
    printf("\n");
}


char is_internal(const char *command,const char **commands_given){
	for(int i=0;i< 20;i++){//20 for adding more commands as you like
		if(strcmp(commands_given[i],command)==0){
			return 1;
		}
	}
	return 0;
}


int is_external(char *command){
    if (command == NULL) {
        fprintf(stderr, "Error: Command is NULL.\n");
        return 0; // Command is not external
    }

    // Get the PATH environment variable
    char *path_env = getenv("PATH");
    if (path_env == NULL) {
        fprintf(stderr, "Error: PATH environment variable is not set.\n");
        return 0; // Command is not external
    }

    // Duplicate the PATH string to modify it
    char *path = malloc(strlen(path_env) + 1);
    if (path == NULL) {
        perror("Error allocating memory for PATH");
        return 0; // Command is not external
    }
    strcpy(path, path_env);

    // Tokenize the PATH variable by ":" to get each directory
    char *dir = strtok(path, ":");
    while (dir != NULL) {
        // Create the full path for the command in the current directory
        char full_path[PATH_MAX];
        snprintf(full_path, sizeof(full_path), "%s/%s", dir, command);

        // Check if the command is executable in this directory
        struct stat statbuf;
        if (stat(full_path, &statbuf) == 0 && S_ISREG(statbuf.st_mode) && (statbuf.st_mode & S_IXUSR)) {
            free(path); // Free the allocated memory
            return 1; // Command is external
        }

        // Move to the next directory in PATH
        dir = strtok(NULL, ":");
    }

    free(path); // Free the allocated memory
    return 0; // Command is not external
}
// Function to execute a command
void execute_command(char command[][50], int count) {
    if (count > 0) {
        // For other commands, search in PATH
        char *path_env = getenv("PATH");
        if (path_env == NULL) {
            fprintf(stderr, "Error: PATH environment variable is not set.\n");
            return;
        }

        // Duplicate the PATH string to modify it
        char *path = malloc(strlen(path_env) + 1);
        if (path == NULL) {
            perror("Error allocating memory for PATH");
            return;
        }
        strcpy(path, path_env);

        // Tokenize the PATH variable by ":" to get each directory
        char *dir = strtok(path, ":");
        while (dir != NULL) {
            // Create the full path for the command in the current directory
            
            char full_path[PATH_MAX];
            snprintf(full_path, sizeof(full_path), "%s/%s", dir, command[0]);

            // Check if the command is executable in this directory
            if (access(full_path, X_OK) == 0) {
                // Fork and execute the command
               
                pid_t pid = fork();
                if (pid < 0) {
                    perror("Error forking");
                    free(path);
                    return;
                } else if (pid == 0) {
                    // Child process
                    // Prepare arguments for execv
                    char *argv[count + 1];
                    for (int i = 0; i < count; i++) {
                        argv[i] = command[i];
                    }
                    argv[count] = NULL; // execv requires a NULL-terminated array

                    execv(full_path, argv);
                    // If execv returns, an error occurred
                    perror("Error executing command");
                    exit(EXIT_FAILURE);
                } else {
                    // Parent process
                    int status;
                    waitpid(pid, &status, 0); // Wait for child process to complete
                    if (WIFEXITED(status)) {
                        printf("Child exited with status %d\n", WEXITSTATUS(status));
                    }
                    free(path);  // Free allocated memory
                    return;
                }
            }

            // Move to the next directory in PATH
            dir = strtok(NULL, ":");
        }
        fprintf(stderr, "Error: Command not found in PATH so not external command.\n");
        
        free(path);  // Ensure allocated memory is freed
    }
}









