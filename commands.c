/*
Author: Assem Samy
File: commands.c
Comment: Contains all function definitions used in shell.c
*/

/*-----------------Includes Section--------------*/
#include "commands.h"

/*-----------Section Contains Function Definitions Used in shell.c File------------*/

/* Function to copy a file from source to destination.
 * The 'append' flag determines if the file should be appended to.
 */
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

    // Determine the flags to use for opening the destination file
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

/* Function to move a file from source to destination directory.
 * 'force' flag determines if existing files in the destination should be overwritten.
 */
void move_file(const char *source, const char *destination_dir, int force) {
    char destination[PATH_MAX];

    // Construct the full destination path
    snprintf(destination, sizeof(destination), "%s/%s", destination_dir, basename(source));

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
}

/* Function to remove a file from the filesystem */
void remove_file(const char *path) {
    // Attempt to remove the file using unlink
    if (unlink(path) != 0) {
        perror("unlink");
        exit(EXIT_FAILURE);
    }
}

/* Function to print all environment variables */
void env_command() {
    int i = 0;
    while (environ[i] != NULL) {
        printf("%s\n", environ[i]);
        i++;
    }
}

/* Function to change the current working directory.
 * Handles ".." to go up one directory level.
 */
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

/* Function to print arguments provided to 'echo' command */
void echo_command(int arg_count, char args[][50]) {
    for (int i = 1; i < arg_count; i++) {
        if (args[i] != NULL) {
            printf("%s ", args[i]);
        }
    }
    printf("\n");
}

/* Function to check if a command is internal */
char is_internal(const char *command, const char **commands_given) {
    for (int i = 0; i < 20; i++) { // 20 for adding more commands if needed
        if (commands_given[i] != NULL) {
            if (strcmp(commands_given[i], command) == 0) {
                return 1; // Command is internal
            }
        } else {
            break;
        }
    }
    return 0; // Command is not internal
}

/* Function to check if a command is external */
int is_external(char *command) {
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

/* Function to execute a command */
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

/* Function to display memory usage statistics */
void free_command() {
    FILE *file = fopen("/proc/meminfo", "r");
    if (file == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    char line[BUF_SIZE_MEM];
    unsigned long total_memory = 0, free_memory = 0, available_memory = 0;
    unsigned long buffers = 0, cached = 0, swap_total = 0, swap_free = 0;

    while (fgets(line, sizeof(line), file)) {
        if (strncmp(line, "MemTotal:", 9) == 0) {
            sscanf(line, "MemTotal: %lu kB", &total_memory);
        } else if (strncmp(line, "MemFree:", 8) == 0) {
            sscanf(line, "MemFree: %lu kB", &free_memory);
        } else if (strncmp(line, "MemAvailable:", 13) == 0) {
            sscanf(line, "MemAvailable: %lu kB", &available_memory);
        } else if (strncmp(line, "Buffers:", 8) == 0) {
            sscanf(line, "Buffers: %lu kB", &buffers);
        } else if (strncmp(line, "Cached:", 7) == 0) {
            sscanf(line, "Cached: %lu kB", &cached);
        } else if (strncmp(line, "SwapTotal:", 10) == 0) {
            sscanf(line, "SwapTotal: %lu kB", &swap_total);
        } else if (strncmp(line, "SwapFree:", 9) == 0) {
            sscanf(line, "SwapFree: %lu kB", &swap_free);
        }
    }

    fclose(file);

    unsigned long used_memory = total_memory - free_memory;
    unsigned long buff_cache = buffers + cached;

    printf("              total        used        free      shared  buff/cache   available\n");
    printf("Mem:       %8lu 	%8lu   %8lu %8lu    %8lu     %8lu\n",
           total_memory,
           used_memory,
           free_memory,
           0,  // shared memory is not provided by sysinfo
           buff_cache,
           available_memory);

    printf("Swap:      %8lu %8lu 	   %8lu\n",
           swap_total,
           swap_total - swap_free,
           swap_free);
}

/* Function to get the system uptime and current time */
void get_uptime_with_time() {
    FILE *file = fopen("/proc/uptime", "r");
    if (file == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    double uptime_seconds = 0;
    double idle_seconds = 0;

    if (fscanf(file, "%lf %lf", &uptime_seconds, &idle_seconds) != 2) {
        perror("fscanf");
        fclose(file);
        exit(EXIT_FAILURE);
    }
    fclose(file);

    // Get current time
    time_t now = time(NULL);
    struct tm *local_time = localtime(&now);
    
    if (local_time == NULL) {
        perror("localtime");
        exit(EXIT_FAILURE);
    }

    // Convert uptime to days, hours, minutes, and seconds
    long uptime_days = (long)(uptime_seconds / (24 * 3600));
    uptime_seconds -= uptime_days * 24 * 3600;
    long uptime_hours = (long)(uptime_seconds / 3600);
    uptime_seconds -= uptime_hours * 3600;
    long uptime_minutes = (long)(uptime_seconds / 60);
    long uptime_seconds_final = (long)(uptime_seconds - uptime_minutes * 60);

    // Print current time
    printf("Current time: %04d-%02d-%02d %02d:%02d:%02d\n",
           local_time->tm_year + 1900, local_time->tm_mon + 1, local_time->tm_mday,
           local_time->tm_hour, local_time->tm_min, local_time->tm_sec);

    // Print uptime
    printf("Uptime: %ld days, %ld hours, %ld minutes, %ld seconds\n",
           uptime_days, uptime_hours, uptime_minutes, uptime_seconds_final);
}

/* Function to execute two commands with a pipe between them */
void pipe_commands(char *cmd1, char *cmd2) {
    int pipefd[2];
    pid_t pid1, pid2;

    // Create a pipe
    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    // Fork the first child process
    if ((pid1 = fork()) == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid1 == 0) {
        // First child process
        close(pipefd[0]); // Close unused read end
        dup2(pipefd[1], STDOUT_FILENO); // Redirect stdout to pipe
        close(pipefd[1]); // Close original write end

        // Tokenize and execute the first command
        char *args1[MAX_TOKENS];
        int i = 0;
        char *token = strtok(cmd1, " \n");
        while (token != NULL && i < MAX_TOKENS - 1) {
            args1[i++] = token;
            token = strtok(NULL, " \n");
        }
        args1[i] = NULL; // Null-terminate the argument list

        execvp(args1[0], args1);
        perror("execvp");
        exit(EXIT_FAILURE);
    }

    // Fork the second child process
    if ((pid2 = fork()) == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid2 == 0) {
        // Second child process
        close(pipefd[1]); // Close unused write end
        dup2(pipefd[0], STDIN_FILENO); // Redirect stdin to pipe
        close(pipefd[0]); // Close original read end

        // Tokenize and execute the second command
        char *args2[MAX_TOKENS];
        int i = 0;
        char *token = strtok(cmd2, " \n");
        while (token != NULL && i < MAX_TOKENS - 1) {
            args2[i++] = token;
            token = strtok(NULL, " \n");
        }
        args2[i] = NULL; // Null-terminate the argument list

        execvp(args2[0], args2);
        perror("execvp");
        exit(EXIT_FAILURE);
    }

    // Parent process
    close(pipefd[0]);
    close(pipefd[1]);
    wait(NULL); // Wait for both children to finish
    wait(NULL);
}

