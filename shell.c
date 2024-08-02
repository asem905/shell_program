
#include "commands.h"

int main(int argc, char *argv[]) {
    char command[100];
    const char *commands_given[] = {"mypwd", "myecho", "mycp", "mymv","myexit","myrm","myhelp","envir","mycd","mytype"};
    char token[30][50];  // Array to store tokens as strings
    ssize_t readsize = 0;
    const char *shellmsg = "Here we go!!\n";
    char buff[PATH_MAX];  // Buffer for getcwd
    char *retval;

    write(STDOUT, shellmsg, strlen(shellmsg));

while(1){
   memset(command, '\0', sizeof(command)); 
   readsize = read(STDIN, command, sizeof(command)-1);  // Use sizeof(command)-1 to leave space for null terminator
    if (readsize == -1) {
        perror("read failed\n");
        exit(EXIT_FAILURE);
    }

    // Ensure null termination of command string
    command[readsize-1] = '\0';

    // Tokenize command
    char *tok = strtok(command, " ");
    int i = 0;
    while (tok != NULL && i < 100) {
            strncpy(token[i], tok, sizeof(token[i]) - 1);
            token[i][sizeof(token[i]) - 1]='\0';
            i++;
            tok = strtok(NULL, " ");
    }
    token[i][0] = '\0';  // Null-terminate the last token
	for (int j = 0; j < i; j++) {
    		printf("Token[%d]: '%s'\n", j, token[j]);
	}
	printf("number of tokens:%d\n",i);
    if (i > 0) {
    	if (((strcmp(token[1], "mymv") == 0) || (strcmp(token[1], "mycp") == 0) || (strcmp(token[1], "mycd") == 0) ||(strcmp(token[1], "myexit") == 0) ||(strcmp(token[1], "myhelp") == 0) ||(strcmp(token[1], "envir") == 0) ||(strcmp(token[1], "mypwd") == 0) ||(strcmp(token[1], "myecho") == 0)) && (strcmp(token[0], "myhelp") != 0) && (strcmp(token[0], "mytype") != 0)){
      		printf("can't use two commands at once!!\n");
    		exit(EXIT_FAILURE);
      }
        if (strcmp(token[0], "mypwd") == 0) {
            retval = getcwd(buff, PATH_MAX);
            if (retval == NULL) {
                perror("getcwd");
                exit(EXIT_FAILURE);
                
            } else {
                printf("%s\n", retval);

            }
        } else if (strcmp(token[0], "myecho") == 0) {
		echo_command(i, token); 

        } else if (strcmp(token[0], "mycp") == 0) {
            int append = 0;
            int source_index = 0;
            int destination_index = 0;

            // Check for -a option
            

            
	    for(int k=1;k<i-1;k++){
	    	    source_index = k;
	    	    if (i > 1 && strcmp(token[1], "-a") == 0) {
		        append = 1;
		        source_index = k+1;
		        
		    }
		    destination_index = i-1;
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
            int force=0;
            int destination_index = i-1;
            if (i < destination_index) {
                fprintf(stderr, "error: mv <source> <destination>\n");
                exit(EXIT_FAILURE);
            }

	    int k=1;
	    if ((i > 1 && strcmp(token[1], "-f") == 0)){
		    	 force=1;
		    	 k=2;
	    }
            for(int j=k;j<i-1;j++){
		    
            	    const char *source = token[j];
		    const char *destination_dir = token[destination_index];

		    // Debug print to verify source and destination
		    printf("Moving file %s to %s\n", source, destination_dir);
		    move_file(source, destination_dir,force);
		    printf("File %s moved successfully to %s\n", source, destination_dir);
            }	

            printf("Move operation completed.\n");
            
        }else if (strcmp(token[0], "myexit") == 0){
        	printf("good bye \n");
    		exit(EXIT_SUCCESS);
        }else if(strcmp(token[0], "myrm") == 0){
        	if (i < 2) {
                    fprintf(stderr, "error: rm <file>\n");
                    exit(EXIT_FAILURE);
                }
                
                const char *file_path = token[1];

                // Debug print to verify file path
                printf("Removing file %s\n", file_path);
                remove_file(file_path);
                printf("File %s removed successfully\n", file_path);

        } else if(strcmp(token[0], "myhelp") == 0){
        	if (i < 2) {
                    fprintf(stderr, "error: rm <file>\n");
                    exit(EXIT_FAILURE);
                }
        	if(strcmp(token[1], "myecho") == 0){
        		printf("echo:used to print what you give it on terminal\n");
        	}else if(strcmp(token[1], "mycp") == 0){
        		printf("cp:used to copy what you give it as source to destination given\n");
        	}else if(strcmp(token[1], "mymv") == 0){
        		printf("mv:used to move what you give it as source to destination given\n");
        	}else if(strcmp(token[1], "mypwd") == 0){
        		printf("pwd:used to print what directory you are at\n");
        	}else if(strcmp(token[1], "myrm") == 0){
        		printf("rm:used to remove file given\n");
        	}else if(strcmp(token[1], "myexit") == 0){
        		printf("exit:used to exit from file\n");
        	}else if(strcmp(token[1], "envir") == 0){
        		printf("env:used to display enviroment variables\n");
        	}else if(strcmp(token[1], "mycd") == 0){
        		printf("cd:used to go to whatever directory you want\n");
        	
        	}else if(strcmp(token[1], "mytype") == 0){
        		printf("mytype:used to give us type of command you insert\n");
        	
        	}else if(strcmp(token[1], NULL) == 0){printf("nothing is given!!\n");}
        
      }else if(strcmp(token[0], "envir") == 0){
      
		env_command();
      }else if(strcmp(token[0], "mycd") == 0){
      
		cd_command(token[1]);
      }else if(strcmp(token[0], "mytype") == 0){
      
		if(is_internal(token[1],commands_given)){
			printf("internal command!!\n");
		}else if(is_external(token[1])){
			printf("external command!!\n");
		}else{
			printf("unsupported command!!\n");
		}
      
      }else{
      		execute_command(token,i);
      		
      		
    		
      }
      
    }
}
    return 0;
}


