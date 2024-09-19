/*
Author: Assem Samy
File: commands.c
Comment: Contains all function definitions used in shell.c
*/

/-----------------Includes Section--------------/
#include "commands.h"
/-----------------static functions concerning ls---------------/
typedef struct {
    char name[TOKEN_LENGTH];
    time_t time; // Generic time field
    int type;    // Type of time (0 for change time, 1 for modification time, 2 for access time)
    int sort_type;// 0 for time, 1 for name
} FileEntry;

// Comparison functions for different types of times
static int compare_time(const void *a, const void *b) {
    const FileEntry *entryA = (const FileEntry *)a;
    const FileEntry *entryB = (const FileEntry *)b;

    // Compare times (descending order)
    if (entryA->time < entryB->time) return 1;
    if (entryA->time > entryB->time) return -1;
    return 0;
}

static void usageError(const char *progName, const char *msg, int opt) {
    fprintf(stderr, "%s: %s -%c\n", progName, msg, opt);
    exit(EXIT_FAILURE);
}

static int compare_name(const void *p1, const void *p2) {
    const FileEntry *entryA = (const FileEntry *)p1;
    const FileEntry *entryB = (const FileEntry *)p2;

    return strcmp(entryA->name, entryB->name);
}

static int is_directory(const char *path) {
    struct stat path_stat;
    if (stat(path, &path_stat) != 0) {
        //perror("stat");
        return 0; // Error or does not exist
    }
    return S_ISDIR(path_stat.st_mode);
}

static void do_ls_directory(char *dir, int hidden_files, int sorting_required, int line_by_line) {
    struct dirent *entry;
    FileEntry entries[MAX_ENTRIES];
    DIR *dp = opendir(dir);

    if (dp == NULL) {
        fprintf(stderr, "Cannot open directory: %s\n", dir);
        return;
    }

    errno = 0;
    int count = 0;
    while ((entry = readdir(dp)) != NULL) {
        if (count >= MAX_ENTRIES) {
            fprintf(stderr, "Too many entries\n");
            break;
        }


        if (!hidden_files && (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)) {
            continue;
        }

        strncpy(entries[count].name, entry->d_name, TOKEN_LENGTH - 1);
        entries[count].name[TOKEN_LENGTH - 1] = '\0'; // Ensure null-termination

        count++;
    }

    closedir(dp);

    if (sorting_required) {
        // Sort entries by name
        qsort(entries, count, sizeof(FileEntry), compare_name);
    }

    if (line_by_line) {
        // Print entries line by line
        for (int i = 0; i < count; i++) {
            printf("%s\n", entries[i].name);
        }
    } else {
        // Determine maximum width for column alignment
        int name_width = 0;
        for (int i = 0; i < count; i++) {
            int len = strlen(entries[i].name);
            if (len > name_width) {
                name_width = len;
            }
        }
        name_width += 4; // Add extra space for readability

        // Calculate number of columns based on terminal width
        int terminal_width = 80; // Adjust terminal width as needed
        int cols = terminal_width / name_width;  

        // Print entries in columns
        for (int i = 0; i < count; i++) {
            printf("%-*s", name_width, entries[i].name);
            if ((i + 1) % cols == 0 || i == count - 1) {
                printf("\n");
            }
        }
    }
}

static void show_stat_info_each(char token[][TOKEN_LENGTH], const char *dir, char t_check, int sort_by_name, int hidden_files, int inode_print) {
    struct dirent *entry;
    int capacity = INITIAL_CAPACITY;  // Track current capacity
    FileEntry *entries = malloc(capacity * sizeof(FileEntry));
    if (entries == NULL) {
        perror("malloc failed");
        return;
    }

    DIR *dp = opendir(dir);
    if (dp == NULL) {
        fprintf(stderr, "Cannot open directory: %s\n", dir);
        free(entries);
        return;
    }

    errno = 0;
    int count = 0;
    while ((entry = readdir(dp)) != NULL) {
        if (count >= capacity) {
            // Resize the array if necessary
            capacity *= 2;
            FileEntry *new_entries = realloc(entries, capacity * sizeof(FileEntry));
            if (new_entries == NULL) {
                perror("realloc failed");
                free(entries);
                closedir(dp);
                return;
            }
            entries = new_entries;
        }

        // Skip special entries like '.' and '..' unless hidden_files is true
        if (!hidden_files && (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)) {
            continue;
        }

        strncpy(entries[count].name, entry->d_name, TOKEN_LENGTH - 1);
        entries[count].name[TOKEN_LENGTH - 1] = '\0';  // Ensure null-termination

        char path[PATH_MAX];
        snprintf(path, sizeof(path), "%s/%s", dir, entries[count].name);
        struct stat info;
        if (lstat(path, &info) == -1) {
            perror("lstat failed");
            continue;
        }

        // Only set time if t_check is valid ('c', 'm', or 'a')
        if (t_check == 'c') {
            entries[count].time = info.st_ctime;
        } else if (t_check == 'm') {
            entries[count].time = info.st_mtime;
        } else if (t_check == 'a') {
            entries[count].time = info.st_atime;
        } else {
            // Skip setting the time if t_check is not valid
            entries[count].time = 0; // Optional: Set to 0 or leave uninitialized depending on your design
        }

        count++;
    }
    closedir(dp);

    // Sort entries based on the chosen criterion
    if (sort_by_name) {
        qsort(entries, count, sizeof(FileEntry), compare_name);
    } else if (t_check == 'c' || t_check == 'm' || t_check == 'a') {
        qsort(entries, count, sizeof(FileEntry), compare_time);
    } else {
        // No sorting happens if t_check is invalid and sort_by_name is not selected
        // You can add a log here to inform the user if needed.
    }

    for (int j = 0; j < count; j++) {
        char path[PATH_MAX];
        char timebuf[20] = {0};  // Initialize time buffer to avoid undefined behavior

        snprintf(path, sizeof(path), "%s/%s", dir, entries[j].name);
        struct stat info;
        if (lstat(path, &info) == -1) {
            perror("lstat failed");
            continue;
        }

        int mode = info.st_mode;
        char str[11] = "----------";
        struct group *grp = getgrgid(info.st_gid);
        struct passwd *pwd = getpwuid(info.st_uid);

        if (grp == NULL) {
            printf("Unknown group ID: %d\n", info.st_gid);
        }
        if (pwd == NULL) {
            printf("Unknown user ID: %d\n", info.st_uid);
        }

        if (S_ISDIR(mode)) str[0] = 'd';
        else if (S_ISLNK(mode)) str[0] = 'l';

        if (mode & S_IRUSR) str[1] = 'r';
        if (mode & S_IWUSR) str[2] = 'w';
        if (mode & S_IXUSR) str[3] = 'x';

        if (mode & S_IRGRP) str[4] = 'r';
        if (mode & S_IWGRP) str[5] = 'w';
        if (mode & S_IXGRP) str[6] = 'x';

        if (mode & S_IROTH) str[7] = 'r';
        if (mode & S_IWOTH) str[8] = 'w';
        if (mode & S_IXOTH) str[9] = 'x';

        struct tm *tm_info = localtime(&entries[j].time);
        if (strftime(timebuf, sizeof(timebuf), "%Y-%m-%d %H:%M", tm_info) == 0) {
            perror("strftime failed");
            timebuf[0] = '\0';  // Set empty string if formatting fails
        }

        if (!inode_print) {
            printf("%-*s   %-*ld %-*s %-*s %-*ld %-*s %-*s\n",
                   10, str,
                   4, info.st_nlink,
                   5, pwd ? pwd->pw_name : "UNKNOWN",
                   5, grp ? grp->gr_name : "UNKNOWN",
                   8, info.st_size,
                   20, timebuf,
                   30, entries[j].name);
        } else {
            printf("%-8lu %-*s   %-*ld %-*s %-*s %-*ld %-*s %-*s\n",
                   (unsigned long)info.st_ino,
                   10, str,
                   4, info.st_nlink,
                   5, pwd ? pwd->pw_name : "UNKNOWN",
                   5, grp ? grp->gr_name : "UNKNOWN",
                   8, info.st_size,
                   20, timebuf,
                   30, entries[j].name);
        }
    }

    free(entries);
}




static void show_directory(char token[][TOKEN_LENGTH], char *dir, int l_check, int inode_print) {
    
    struct dirent *entry;
    DIR *dp = opendir(dir);
    if (dp == NULL) {
        fprintf(stderr, "Cannot open directory: %s\n", dir);
        return;
    }

    // Column widths
    const int PERMS_WIDTH = 10;
    const int LINKS_WIDTH = 4;
    const int UID_WIDTH = 5;
    const int GID_WIDTH = 5;
    const int SIZE_WIDTH = 8;
    const int TIME_WIDTH = 20;
    const int NAME_WIDTH = 30;


        struct stat info;
        char path[PATH_MAX];
        char timebuf[20];

        // Initialize timebuf to ensure it's all zeroes
        memset(timebuf, '\0', sizeof(timebuf));

        // Print the directory itself
        snprintf(path, sizeof(path), "%s", dir);
        if (lstat(path, &info) == -1) {
            perror("lstat failed");
            closedir(dp);
            return;
        }

        int mode = info.st_mode;
        char str[11] = "----------";
        struct group *grp = getgrgid(info.st_gid);
        struct passwd *pwd = getpwuid(info.st_uid);

        // File type
        if (S_ISDIR(mode)) str[0] = 'd';
        else if (S_ISLNK(mode)) str[0] = 'l';

        // Owner permissions
        if (mode & S_IRUSR) str[1] = 'r';
        if (mode & S_IWUSR) str[2] = 'w';
        if (mode & S_IXUSR) str[3] = 'x';

        // Group permissions
        if (mode & S_IRGRP) str[4] = 'r';
        if (mode & S_IWGRP) str[5] = 'w';
        if (mode & S_IXGRP) str[6] = 'x';

        // Others permissions
        if (mode & S_IROTH) str[7] = 'r';
        if (mode & S_IWOTH) str[8] = 'w';
        if (mode & S_IXOTH) str[9] = 'x';

        // Special permissions
        if (mode & S_ISUID) str[2] = (str[2] == 'x') ? 's' : 'S';
        if (mode & S_ISGID) str[5] = (str[5] == 'x') ? 's' : 'S';
        if (mode & S_ISVTX) str[8] = (str[8] == 'x') ? 't' : 'T';

        // Format the time based on the type
        struct tm *tm_info = localtime(&info.st_mtime);
        if (strftime(timebuf, sizeof(timebuf), "%Y-%m-%d %H:%M", tm_info) == 0) {
            perror("strftime failed");
            timebuf[0] = '\0'; // In case of failure, provide an empty string
        }
	if (!l_check) {
		if (!inode_print) {
        		printf("%s\n", dir);
        	}else{
        		printf("%-8lu %-*s\n", (unsigned long)info.st_ino, NAME_WIDTH, dir);
        	}
        }else{
		if (!inode_print) {
		    // Print file info
		    printf("%-*s   %-*ld %-*s %-*s %-*ld %-*s %-*s\n",
		           PERMS_WIDTH, str,
		           LINKS_WIDTH, info.st_nlink,
		           UID_WIDTH, pwd ? pwd->pw_name : "UNKNOWN",
		           GID_WIDTH, grp ? grp->gr_name : "UNKNOWN",
		           SIZE_WIDTH, info.st_size,
		           TIME_WIDTH, timebuf,
		           NAME_WIDTH, dir);
		} else {
		    printf("%-8lu %-*s   %-*ld %-*s %-*s %-*ld %-*s ",
		           (unsigned long)info.st_ino,
		           PERMS_WIDTH, str,
		           LINKS_WIDTH, info.st_nlink,
		           UID_WIDTH, pwd ? pwd->pw_name : "UNKNOWN",
		           GID_WIDTH, grp ? grp->gr_name : "UNKNOWN",
		           SIZE_WIDTH, info.st_size,
		           TIME_WIDTH, timebuf
		           );
		    printf("%-*s\n", NAME_WIDTH, dir);
		}
	}
    
    closedir(dp);
}
static void show_inode_number(char token[][TOKEN_LENGTH], char *dir, int inode_long_format_check, int line_by_line,int sorting_required) {
    struct dirent *entry;
    FileEntry entries[1000];

    DIR *dp = opendir(dir);
    if (dp == NULL) {
        fprintf(stderr, "Cannot open directory: %s\n", dir);
        return;
    }

    errno = 0;
    int count = 0;
    char mode_time=0;
    while ((entry = readdir(dp)) != NULL) {
        if (count >= sizeof(entries) / sizeof(entries[0])) {
            fprintf(stderr, "Too many entries\n");
            break;
        }
        
        // Skip special entries like '.' and '..'
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        // Copy entry name
        strncpy(entries[count].name, entry->d_name, TOKEN_LENGTH - 1);
        entries[count].name[TOKEN_LENGTH - 1] = '\0'; // Ensure null-termination

        // Build the full path for the file
        char path[PATH_MAX];
        snprintf(path, sizeof(path), "%s/%s", dir, entries[count].name);
        struct stat info;
        if (lstat(path, &info) == -1) {
            perror("lstat failed");
            continue; // Skip this entry if lstat fails
        }
        // Store inode number in the time field
        entries[count].time = info.st_ino;
        count++;
    }
    closedir(dp);

    // Sort entries by name
    if(sorting_required){
    	qsort(entries, count, sizeof(FileEntry), compare_name);
    }else{
    }
    // Determine maximum widths for formatting
    int name_width = 0;
    for (int j = 0; j < count; j++) {
        if (strlen(entries[j].name) > name_width) {
            name_width = strlen(entries[j].name);
        }
    }
    name_width += 4; // Add extra space for better readability

    if (line_by_line) {
        for (int j = 0; j < count; j++) {
            printf("%-*lu %s\n", name_width, (unsigned long)entries[j].time, entries[j].name);
        }
    } else {
        // Print entries in columns
        int cols = 80 / COLUMN_WIDTH;  // Assuming terminal width is 80 columns
        for (int i = 0; i < count; i++) {
            // Print entry with aligned columns
            printf("%-*lu %-*s", 10, (unsigned long)entries[i].time, name_width, entries[i].name);
            if ((i + 1) % cols == 0 || i == count - 1) {
                printf("\n");
            }
        }
    }
}
static void sort_files(char token[][TOKEN_LENGTH],const char *dir, int time_type,int line_by_line,int hidden_files) {
    struct dirent *entry;
    FileEntry entries[1000];
    DIR *dp = opendir(dir);
    if (dp == NULL) {
        fprintf(stderr, "Cannot open directory: %s\n", dir);
        return;
    }

    errno = 0;
    int count = 0;
    while ((entry = readdir(dp)) != NULL) {
        if (count >= sizeof(entries) / sizeof(entries[0])) {
            fprintf(stderr, "Too many entries\n");
            break;
        }

        // Skip special entries like '.' and '..'
        if (!hidden_files && (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)) {
            continue;
        }

        strncpy(entries[count].name, entry->d_name, TOKEN_LENGTH - 1);
        entries[count].name[TOKEN_LENGTH - 1] = '\0'; // Ensure null-termination

        // Build the full path for the file and get its time
        char path[PATH_MAX];
        snprintf(path, sizeof(path), "%s/%s", dir, entries[count].name);
        struct stat info;
        if (lstat(path, &info) == -1) {
            perror("lstat failed");
            continue;
        }

        // Set the appropriate time based on time_type
        switch (time_type) {
            case 0: // Change time
                entries[count].time = info.st_ctime;
                break;
            case 1: // Modification time
                entries[count].time = info.st_mtime;
                break;
            case 2: // Access time
                entries[count].time = info.st_atime;
                break;
            default:
                fprintf(stderr, "Invalid time type\n");
                closedir(dp);
                return;
        }
        entries[count].type = time_type;
        count++;
    }
    closedir(dp);

    // Sort entries by the selected time type
    qsort(entries, count, sizeof(FileEntry), compare_time);
    // Determine maximum widths for formatting
    int name_width = 0;
    for (int j = 0; j < count; j++) {
        if (strlen(entries[j].name) > name_width) {
            name_width = strlen(entries[j].name);
        }
    }
    name_width += 4;
    if (line_by_line) {
        for (int j = 0; j < count; j++) {
            printf("%s\n",entries[j].name);
        }
    } else {
        // Print entries in columns
        int cols = 80 / COLUMN_WIDTH;  // Assuming terminal width is 80 columns
        for (int i = 0; i < count; i++) {
            // Print entry with aligned columns
            printf("%-*s", name_width, entries[i].name);
            if ((i + 1) % cols == 0 || i == count - 1) {
                printf("\n");
            }
        }
    }
}








/-----------Section Contains Function Definitions Used in shell.c File------------/

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

void execute_command(char command[][TOKEN_LENGTH], int count) {
    if (count > 0) {
        // Fork a new process to execute the command
        pid_t pid = fork();
        if (pid < 0) {
            perror("Error forking");
            return;
        } else if (pid == 0) {
            // Child process

            // Prepare arguments for execvp
            char *argv[count ];
            for (int i = 0; i < count; i++) {
                argv[i] = strdup(command[i]);  // Duplicate the command arguments
                printf("Token[%d]: '%s'\n", i, argv[i]);  // Debugging line to check the arguments
            }
            argv[count] = NULL; // execvp requires a NULL-terminated array

            // Print command to be executed (for debugging)
            printf("Executing command: %s\n", argv[0]);

            // Execute the command using execvp, which searches for the command in PATH
            if (execvp(argv[0], argv) == -1) {
                perror("Error executing command");
            }

            // If execvp returns, an error occurred
            exit(EXIT_FAILURE);
        } else {
            // Parent process
            int status;
            waitpid(pid, &status, 0); // Wait for the child process to complete
            if (WIFEXITED(status)) {
                printf("Child exited with status %d\n", WEXITSTATUS(status));
            }
        }
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





void ls_command_implementation(int argc, char token[][TOKEN_LENGTH]) {
   // Initialize variables for options and settings
    int hidden_files = 0;  // Option to show hidden files
    int line_by_line = NOLINE_BYLINE_PRINT;  // Option to print one file per line
    int long_format = 0;  // Option for long format listing
    int inode_check = 0;  // Option to display inode numbers
    int sort_time = NOSORT_BY_TIME;  // Sort by time option (not enabled by default)
    int sort_name = NOSORT_BY_NAME;  // Sort by name option (not enabled by default)
    int dir_num = -1;  // Index for the specified directory (if any)
    int sorting_required = NOSORTING_REQUIRED;  // Flag to check if sorting is required
    int directory_display = 0;  // Option to display directory info only
    char *optstring = "aldtiuc1f";  // Supported option string for getopt
    int opt;  // Variable to hold option returned by getopt
    int t_time = 0;  // Flag for time-based sorting
    int l_check = 0;  // Flag to track long format
    int f_check = 0;  // Flag to check for 'f' option
    int c_check = 0;  // Flag to check for 'c' option
    int u_check = 0;  // Flag to check for 'u' option
    
    // Step 1: Calculate the total number of options and arguments
    int opt_count = 0;
    for (int i = 0; i < argc; i++) {
        if (token[i][0] == '-') {
            opt_count += strlen(token[i]) - 1;  // Count the options in each token
        }
    }

    // Step 2: Create an argv array large enough to hold individual options
    char *argv[argc + opt_count];
    int argv_index = 0;

    // Loop through each token to handle options and directory names
    for (int i = 0; i < argc; i++) {
        if (token[i][0] == '-') {
            // Split options like "-liatc" into individual arguments "-l", "-i", "-a", etc.
            for (int j = 1; j < strlen(token[i]); j++) {
                argv[argv_index] = (char *)malloc(3 * sizeof(char));  // Allocate memory for each option "-x"
                if (argv[argv_index] == NULL) {
                    fprintf(stderr, "Memory allocation failed\n");
                    return;
                }
                snprintf(argv[argv_index], 3, "-%c", token[i][j]);  // Create each option argument
                argv_index++;
            }
        } else {
            // If the token is not an option, treat it as a directory/file name
            argv[argv_index] = (char *)malloc(TOKEN_LENGTH * sizeof(char));  // Allocate memory for file/directory name
            if (argv[argv_index] == NULL) {
                fprintf(stderr, "Memory allocation failed\n");
                return;
            }
            strncpy(argv[argv_index], token[i], TOKEN_LENGTH - 1);  // Copy the name into argv
            argv[argv_index][TOKEN_LENGTH - 1] = '\0';  // Ensure null termination
            argv_index++;
        }
    }
    // Step 3: Set the correct argc value for getopt
    argc = argv_index;
    optind = 1;
    // Using getopt to parse the options
    while ((opt = getopt(argc, argv, optstring)) != -1) {
        switch (opt) {
            case 'a':
                hidden_files = 1;
                break;
            case 'l':
            	l_check=1;
                long_format = 1;
                sorting_required = SORTING_REQUIRED;
                if(t_time){// this is used as except -t found it is sorted alphabetical sorting
                	sort_name = NOSORT_BY_NAME;
                }else{
                	sort_name = SORT_BY_NAME;
                }
                break;
            case 'd':
                directory_display = 1; 
                break;
            case 't':
            	f_check=0;
            	sorting_required = SORTING_REQUIRED;
                if(c_check){
                	sort_time = C_TIME;
                	
                }else if(u_check){
                	sort_time = ACCESS_TIME;
                	
                }else if(sort_time == NOSORT_BY_TIME){
                	sort_time = MODIFICATION_TIME;
                	
                }
                t_time=1;
                sort_name = NOSORT_BY_NAME;
                break;
            case 'i':
                inode_check = 1;
                break;
            case 'u':
                sort_time = ACCESS_TIME;
                f_check=0;
                u_check=1;
                sorting_required = SORTING_REQUIRED;
                if(t_time){
                	
                	sort_name = NOSORT_BY_NAME;
                }else if(l_check ){
                	sort_name = SORT_BY_NAME;
                }else{
                	sort_name = NOSORT_BY_NAME;
                }
                break;
            case 'c':
                sort_time = C_TIME; 
                c_check=1;
                sorting_required = SORTING_REQUIRED;
                f_check=0;
                if(t_time ){
                	sorting_required = SORTING_REQUIRED;
                	sort_name = NOSORT_BY_NAME;
                }else if(l_check){
                	sort_name = SORT_BY_NAME;
                }else{
                	sort_name = NOSORT_BY_NAME;
                }
                break;
            case '1':
                sorting_required = SORTING_REQUIRED;
                line_by_line=LINE_BYLINE_PRINT; 
                break;
            case 'f':
            	hidden_files=1;
            	f_check=1;
                sort_name = NOSORT_BY_NAME;
                sorting_required = NOSORTING_REQUIRED;
                sort_name = NOSORT_BY_NAME;
                if(l_check){
                	long_format = 0;

                }else {
                
                }
                
                break;
            default:
                fprintf(stderr, "Usage: ls [-aldituc1f] [directory ...]\n");
                return;
        }
    }

    // Checking if any directory is specified
    for (int i = 0; i < argc; i++) {
        if (is_directory(argv[i])) {
            dir_num = i;
            break;
        }
    }

    // If no directory is specified, default to the current directory
    const char *directory = (dir_num == -1) ? "." : argv[dir_num];

    // Handling long format with inode check
    if (long_format) {
    	if(directory_display){
    		if (inode_check){
    			show_directory(token,directory,long_format, inode_check ? LONG_FORMAT_CHECK_WITH_INODE : NOLONG_FORMAT_CHECK_WITH_INODE);
    		}else{
    			show_directory(token,directory,long_format, inode_check ? LONG_FORMAT_CHECK_WITH_INODE : NOLONG_FORMAT_CHECK_WITH_INODE);
    		}
    	}else if(!sorting_required && !t_time){
        	show_stat_info_each(token, directory, NOSORT_BY_TIME,inode_check ?SORT_BY_NAME :NOSORT_BY_NAME, hidden_files, inode_check ? LONG_FORMAT_CHECK_WITH_INODE : NOLONG_FORMAT_CHECK_WITH_INODE);
        }else {
        	show_stat_info_each(token, directory, sort_time,sort_name , hidden_files, inode_check ? LONG_FORMAT_CHECK_WITH_INODE : NOLONG_FORMAT_CHECK_WITH_INODE);
        }
    } else if (inode_check && !directory_display ) {
    	if(!line_by_line){
    		if(f_check){
        		show_inode_number(token, directory, NO_INODE_LONG_FORMAT,NOLINE_BYLINE_PRINT, NOSORT_BY_NAME );
        	}else{
        		show_inode_number(token, directory, NO_INODE_LONG_FORMAT,NOLINE_BYLINE_PRINT, SORT_BY_NAME );
        	}
        }else{
        	if(f_check){
        		show_inode_number(token, directory, NO_INODE_LONG_FORMAT,LINE_BYLINE_PRINT, NOSORT_BY_NAME );
        	}else{
        		show_inode_number(token, directory, NO_INODE_LONG_FORMAT,LINE_BYLINE_PRINT, SORT_BY_NAME );
        	}
        }
    }else if (sorting_required && !directory_display) {
    	if(sort_time == MODIFICATION_TIME ){
    		if(!line_by_line){
        		sort_files(token,directory,MODIFICATION_TIME_SORT,NOLINE_BYLINE_PRINT,hidden_files);
        	}else{
        		sort_files(token,directory,MODIFICATION_TIME_SORT,LINE_BYLINE_PRINT,hidden_files);
        	}	
        		
        }else if(sort_time == C_TIME ){
        	if(!line_by_line){
        		sort_files(token,directory,C_TIME_SORT,NOLINE_BYLINE_PRINT,hidden_files);
        	}else{
        		sort_files(token,directory,C_TIME_SORT,LINE_BYLINE_PRINT,hidden_files);
        	}
        }else if(sort_time == ACCESS_TIME ){
        	if(!line_by_line){
        		sort_files(token,directory,ACCESS_TIME_SORT,NOLINE_BYLINE_PRINT,hidden_files);
        	}else{
        		sort_files(token,directory,ACCESS_TIME_SORT,LINE_BYLINE_PRINT,hidden_files);
        	}
        }else if(directory_display){
        	show_directory(token,directory,long_format, inode_check ? LONG_FORMAT_CHECK_WITH_INODE : NOLONG_FORMAT_CHECK_WITH_INODE);
        }else {
        	if(!line_by_line){
        		sort_files(token,directory,SORT_BY_NAME,NOLINE_BYLINE_PRINT,hidden_files);
        	}else{
        		sort_files(token,directory,SORT_BY_NAME,LINE_BYLINE_PRINT,hidden_files);
        	}
        }
        
    } else if (directory_display) {
    	
        show_directory(token,directory,long_format, inode_check ? LONG_FORMAT_CHECK_WITH_INODE : NOLONG_FORMAT_CHECK_WITH_INODE);
    } else if(line_by_line){
        if(f_check){
        	do_ls_directory(directory,  hidden_files, sorting_required,LINE_BYLINE_PRINT);
        }else{
        	sorting_required=SORTING_REQUIRED;
        	do_ls_directory(directory,  hidden_files, sorting_required,LINE_BYLINE_PRINT);
        }
    }else {
        if(f_check){
        	do_ls_directory(directory,  hidden_files, sorting_required,NOLINE_BYLINE_PRINT);
        }else{
        	sorting_required=SORTING_REQUIRED;
        	do_ls_directory(directory,  hidden_files, sorting_required,NOLINE_BYLINE_PRINT);
        }
    }
}
