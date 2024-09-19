#include "commands.h"

int main(int argc, char *argv[]) { 
    char command[100];
    // List of valid commands
    const char *commands_given[] = {"mypwd", "myecho", "mycp", "mymv","myexit","myrm","myhelp","envir","mycd","mytype","myuptime","myfree","|"," myls",NULL};
    char token[MAX_TOKENS][TOKEN_LENGTH];  // Array to store tokens as strings
    ssize_t readsize = 0;
    const char *shellmsg = "Here we go!!\n"; // Message to be displayed when shell starts
    char buff[PATH_MAX];  //for getcwd
    char *retval;
    pid_t pid, wpid;
    int status;
    char *input_file = NULL; // Input redirection file
    char *output_file = NULL; // Output redirection file
    char *error_file = NULL; // Error redirection file
    char *args[MAX_TOKENS]; // Arguments for execvp
    int arg = 1;
    
    // Print initial message
    write(STDOUT, shellmsg, strlen(shellmsg));

    while(1) {
        memset(command, '\0', sizeof(command)); 
        // Read command from standard input
        readsize = read(STDIN, command, sizeof(command)-1);  // Use sizeof(command)-1 to leave space for null terminator
        if (readsize == -1) {
            perror("read failed\n");
            exit(EXIT_FAILURE);
        }

        // Ensure null termination of command string
        command[readsize] = '\0';

        // Tokenize the command input
        char *tok = strtok(command, " \n");
        int i = 0;
        while (tok != NULL && i < MAX_TOKENS) {
            if (strlen(tok) < TOKEN_LENGTH) {
                strncpy(token[i], tok, TOKEN_LENGTH - 1);
                token[i][TOKEN_LENGTH - 1] = '\0'; // Ensure null termination
                i++;
            } else {
                fprintf(stderr, "Token too long, ignoring.\n");
            }
            tok = strtok(NULL, " \n");
        }
        
        token[i][0] = '\0';
        
        // Print tokens for debugging
        for (int j = 0; j < i; j++) {
            printf("Token[%d]: '%s' \n", j, token[j]);
        }
        printf("number of tokens: %d\n", i);
        
        if (i > 0) {
            // Check for multiple commands
            if (i > 1 && (strcmp(token[1], "mymv") == 0 || strcmp(token[1], "mycp") == 0 || strcmp(token[1], "mycd") == 0 ||
                           strcmp(token[1], "myexit") == 0 || strcmp(token[1], "myhelp") == 0 || strcmp(token[1], "envir") == 0 ||
                           strcmp(token[1], "mypwd") == 0 || strcmp(token[1], "myecho") == 0 || strcmp(token[1], "myfree") == 0 ||
                           strcmp(token[1], "myuptime") == 0) && (strcmp(token[0], "myhelp") != 0) && (strcmp(token[0], "mytype") != 0)) {
                printf("can't use two commands at once!!\n");
                continue;
            } 
      
            // Reset redirection file pointers
            input_file = NULL;
            output_file = NULL;
            error_file = NULL;

            // Check for redirection operators
            int found = 0;
            for (int k = 1; k < i; k++) {
                if (strcmp(token[k], "<") == 0) {
                    input_file = token[k + 1];
                    found = 1;
                    k++;  // Skip the next token
                } else if (strcmp(token[k], ">") == 0) {
                    output_file = token[k + 1];
                    k++;  // Skip the next token
                    found = 1;
                } else if (strcmp(token[k], "2>") == 0) {
                    error_file = token[k + 1];
                    k++;  // Skip the next token
                    found = 1;
                }
            }

            // Check for pipe operator
            int pipe_found = 0;
            int pipe_index = -1;
            for (int k = 1; k < i; k++) {
                if (strcmp(token[k], "|") == 0) {
                    pipe_found = 1;
                    pipe_index = k;
                }
            }

            if (i > 0 && pipe_found == 1) {
                // Split input into commands based on '|'
                if (pipe_index > 0 && pipe_index < i - 1) {
                    // Create command strings for both commands around the pipe
                    char command1[100];
                    char command2[100];

                    // Copy commands into separate buffers
                    snprintf(command1, sizeof(command1), "%s", token[0]);
                    for (int k = 1; k < pipe_index; k++) {
                        strncat(command1, " ", sizeof(command1) - strlen(command1) - 1);
                        strncat(command1, token[k], sizeof(command1) - strlen(command1) - 1);
                    }
                    snprintf(command2, sizeof(command2), "%s", token[pipe_index + 1]);
                    for (int k = pipe_index + 2; k < i; k++) {
                        strncat(command2, " ", sizeof(command2) - strlen(command2) - 1);
                        strncat(command2, token[k], sizeof(command2) - strlen(command2) - 1);
                    }
                    
                    // Debug prints to check command1 and command2
                    printf("Command1: '%s'\n", command1);
                    printf("Command2: '%s'\n", command2);

                    // Check if commands are empty
                    if (strlen(command1) > 0 && strlen(command2) > 0) {
                        pipe_commands(command1, command2);
                    } else {
                        fprintf(stderr, "Invalid command. No pipe found.\n");
                        exit(EXIT_FAILURE);
                    }
                } else {
                    fprintf(stderr, "Invalid command format for pipe.\n");
                }
            } else if (i > 0 && found == 1) {
                // Prepare args for execvp
                int arg_index = 0;
                for (int j = 0; j < i; j++) {
                    if (strcmp(token[j], "<") == 0 || strcmp(token[j], ">") == 0 || strcmp(token[j], "2>") == 0) {
                        // Skip redirection tokens and their arguments
                        j++;
                    } else {
                        args[arg_index++] = token[j];
                    }
                }
                args[arg_index] = NULL;  // Null-terminate the args array

                if ((pid = fork()) == -1) {
                    perror("fork");
                    exit(EXIT_FAILURE);
                } else if (pid == 0) {
                    // Child process
                    // Handle input redirection
                    if (input_file) {
                        int fd = open(input_file, O_RDONLY);
                        if (fd == -1) {
                            perror("open input file");
                            exit(EXIT_FAILURE);
                        }
                        dup2(fd, STDIN_FILENO);
                        close(fd);
                    }

                    // Handle output redirection
                    if (output_file) {
                        int fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
                        if (fd == -1) {
                            perror("open output file");
                            exit(EXIT_FAILURE);
                        }
                        dup2(fd, STDOUT_FILENO);
                        close(fd);
                    }

                    // Handle error redirection
                    if (error_file) {
                        int fd = open(error_file, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
                        if (fd == -1) {
                            perror("open error file");
                            exit(EXIT_FAILURE);
                        }
                        dup2(fd, STDERR_FILENO);
                        close(fd);
                    }

                    // Execute the command
                    execvp(args[0], args);
                    perror("execvp");
                    exit(EXIT_FAILURE);
                } else {
                    // Parent process
                    do {
                        wpid = waitpid(pid, &status, WUNTRACED);
                    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
                }
            } else if (strcmp(token[0], "mypwd") == 0) {
                // Handle 'mypwd' command
                retval = getcwd(buff, PATH_MAX);
                if (retval == NULL) {
                    perror("getcwd");
                    exit(EXIT_FAILURE);
                } else {
                    printf("%s\n", retval);
                }
            } else if (strcmp(token[0], "myecho") == 0) {
                // Handle 'myecho' command
                echo_command(i, token); 

            } else if (strcmp(token[0], "mycp") == 0) {
                // Handle 'mycp' command
                int append = 0;
                int source_index = 0;
                int destination_index = 0;

                // Check for -a option
                for(int k = 1; k < i - 1; k++) {
                    source_index = k;
                    if (i > 1 && strcmp(token[1], "-a") == 0) {
                        append = 1;
                        source_index = k + 1;
                    }
                    destination_index = i - 1;
                    if (i < destination_index) {
                        fprintf(stderr, "error: cp [-a] <source> <destination>\n");
                        exit(EXIT_FAILURE);
                    }
                    const char *source = token[source_index];
                    const char *destination = token[destination_index];

                    // Debug print to verify source and destination
                    printf("Copying file %s to %s (append mode: %d)\n", source, destination, append);
                    copy_file(source, destination, append);
                    printf("File %s copied successfully to %s\n", source, destination);
                    source_index++;
                }
                printf("Copy operation completed.\n");
            } else if (strcmp(token[0], "mymv") == 0) {
                // Handle 'mymv' command
                int force = 0;
                int destination_index = i - 1;
                if (i < destination_index) {
                    fprintf(stderr, "error: mv <source> <destination>\n");
                    exit(EXIT_FAILURE);
                }

                int k = 1;
                if ((i > 1 && strcmp(token[1], "-f") == 0)){
                    force = 1;
                    k = 2;
                }
                for (int j = k; j < i - 1; j++) {
                    const char *source = token[j];
                    const char *destination_dir = token[destination_index];

                    // Debug print to verify source and destination
                    printf("Moving file %s to %s\n", source, destination_dir);
                    move_file(source, destination_dir, force);
                    printf("File %s moved successfully to %s\n", source, destination_dir);
                }  

                printf("Move operation completed.\n");
                
            } else if (strcmp(token[0], "myexit") == 0) {
                // Handle 'myexit' command
                printf("good bye \n");
                exit(EXIT_SUCCESS);
            } else if(strcmp(token[0], "myrm") == 0) {
                // Handle 'myrm' command
                if (i < 2) {
                    fprintf(stderr, "error: rm <file>\n");
                    exit(EXIT_FAILURE);
                }
                
                const char *file_path = token[1];

                // Debug print to verify file path
                printf("Removing file %s\n", file_path);
                remove_file(file_path);
                printf("File %s removed successfully\n", file_path);

            } else if(strcmp(token[0], "myhelp") == 0) {
                // Handle 'myhelp' command
                if (i < 2) {
                    fprintf(stderr, "error: rm <file>\n");
                    exit(EXIT_FAILURE);
                }
                if(strcmp(token[1], "myecho") == 0){
                    printf("echo: used to print what you give it on terminal\n");
                } else if(strcmp(token[1], "mycp") == 0){
                    printf("cp: used to copy what you give it as source to destination given\n");
                } else if(strcmp(token[1], "mymv") == 0){
                    printf("mv: used to move what you give it as source to destination given\n");
                } else if(strcmp(token[1], "mypwd") == 0){
                    printf("pwd: used to print what directory you are at\n");
                } else if(strcmp(token[1], "myrm") == 0){
                    printf("rm: used to remove file given\n");
                } else if(strcmp(token[1], "myexit") == 0){
                    printf("exit: used to exit from file\n");
                } else if(strcmp(token[1], "envir") == 0){
                    printf("env: used to display environment variables\n");
                } else if(strcmp(token[1], "mycd") == 0){
                    printf("cd: used to go to whatever directory you want\n");
                } else if(strcmp(token[1], "mytype") == 0){
                    printf("mytype: used to give us the type of command you insert\n");
                } else if(strcmp(token[1], "myfree") == 0){
                    printf("myfree: used to get memory information\n");
                } else if(strcmp(token[1], "myuptime") == 0){
                    printf("myuptime: used to get time\n");
                } else if(strcmp(token[1], NULL) == 0){
                    printf("nothing is given!!\n");
                }
            } else if(strcmp(token[0], "envir") == 0) {
                // Handle 'envir' command
                env_command();
            } else if(strcmp(token[0], "mycd") == 0) {
                // Handle 'mycd' command
                cd_command(token[1]);
            } else if(strcmp(token[0], "mytype") == 0) {
                // Handle 'mytype' command
                if(is_internal(token[1], commands_given)) {
                    printf("internal command!!\n");
                } else if(is_external(token[1])) {
                    printf("external command!!\n");
                } else {
                    printf("unsupported command!!\n");
                }
            } else if(strcmp(token[0], "myfree") == 0) {
                // Handle 'myfree' command
                free_command();
            } else if(strcmp(token[0], "myuptime") == 0) {
                // Handle 'myuptime' command
                get_uptime_with_time();
            }else if(strcmp(token[0], "myls") == 0) {
                ls_command_implementation(i,token);
            } else {
                // Handle unknown commands
                execute_command(token, i);
            }
        }
    }

    return 0;
}
