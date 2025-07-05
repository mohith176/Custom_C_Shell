#include "reveal.h"

int compare_strings(const void *a, const void *b) {
    return strcmp(*(const char **)a, *(const char **)b);
}

void resolve_path(const char *path, const char *homedir, char *resolved_path) {
    if (strcmp(path, "~") == 0) {
        strcpy(resolved_path, homedir);
    } else if (strcmp(path, ".") == 0) {
        getcwd(resolved_path, 4096);
    } else if (strcmp(path, "..") == 0) {
        getcwd(resolved_path, 4096);
        strcat(resolved_path, "/..");
    } else if (strcmp(path, "-") == 0) {
        // Handle previous directory - for simplicity, using current directory
        getcwd(resolved_path, 4096);
    } else if (path[0] == '~') {
        strcpy(resolved_path, homedir);
        strcat(resolved_path, path + 1);
    } else if (path[0] == '/') {
        strcpy(resolved_path, path);
    } else {
        getcwd(resolved_path, 4096);
        strcat(resolved_path, "/");
        strcat(resolved_path, path);
    }
}

void print_colored_name(const char *filepath, const char *filename) {
    struct stat file_stat;
    
    if (stat(filepath, &file_stat) == -1) {
        printf("%s", filename);
        return;
    }
    
    if (S_ISDIR(file_stat.st_mode)) {
        printf(BLUE "%s" RESET, filename);
    } else if (file_stat.st_mode & S_IXUSR) {
        printf(GREEN "%s" RESET, filename);
    } else {
        printf("%s", filename);
    }
}

void print_file_info(const char *filepath, const char *filename, int show_details) {
    struct stat file_stat;
    
    if (stat(filepath, &file_stat) == -1) {
        perror("stat");
        return;
    }
    
    if (show_details) {
        // Print permissions
        printf("%c", S_ISDIR(file_stat.st_mode) ? 'd' : '-');
        printf("%c", (file_stat.st_mode & S_IRUSR) ? 'r' : '-');
        printf("%c", (file_stat.st_mode & S_IWUSR) ? 'w' : '-');
        printf("%c", (file_stat.st_mode & S_IXUSR) ? 'x' : '-');
        printf("%c", (file_stat.st_mode & S_IRGRP) ? 'r' : '-');
        printf("%c", (file_stat.st_mode & S_IWGRP) ? 'w' : '-');
        printf("%c", (file_stat.st_mode & S_IXGRP) ? 'x' : '-');
        printf("%c", (file_stat.st_mode & S_IROTH) ? 'r' : '-');
        printf("%c", (file_stat.st_mode & S_IWOTH) ? 'w' : '-');
        printf("%c", (file_stat.st_mode & S_IXOTH) ? 'x' : '-');
        
        // Print number of links
        printf(" %2ld", file_stat.st_nlink);
        
        // Print owner
        struct passwd *pw = getpwuid(file_stat.st_uid);
        printf(" %s", pw ? pw->pw_name : "unknown");
        
        // Print group
        struct group *gr = getgrgid(file_stat.st_gid);
        printf(" %s", gr ? gr->gr_name : "unknown");
        
        // Print size
        printf(" %8ld", file_stat.st_size);
        
        // Print modification time
        char time_str[80];
        struct tm *timeinfo = localtime(&file_stat.st_mtime);
        strftime(time_str, sizeof(time_str), "%b %d %H:%M", timeinfo);
        printf(" %s ", time_str);
        
        // Print filename with color
        print_colored_name(filepath, filename);
        printf("\n");
    } else {
        print_colored_name(filepath, filename);
        printf("\n");
    }
}

void reveal_cmd(char **arguments, int narg, char *homedir) {
    int show_all = 0;
    int show_details = 0;
    char target_path[4096] = ".";
    int path_specified = 0;
    
    // Parse arguments
    for (int i = 0; i < narg; i++) {
        if (arguments[i][0] == '-') {
            // Parse flags
            for (int j = 1; j < strlen(arguments[i]); j++) {
                if (arguments[i][j] == 'a') {
                    show_all = 1;
                } else if (arguments[i][j] == 'l') {
                    show_details = 1;
                }
            }
        } else {
            // This is the path
            strcpy(target_path, arguments[i]);
            path_specified = 1;
        }
    }
    
    // If no path specified, use current directory
    if (!path_specified) {
        strcpy(target_path, ".");
    }
    
    // Resolve the path
    char resolved_path[4096];
    resolve_path(target_path, homedir, resolved_path);
    
    // Open directory
    DIR *dir = opendir(resolved_path);
    if (dir == NULL) {
        perror("reveal");
        return;
    }
    
    // Read directory entries
    struct dirent *entry;
    char **filenames = malloc(sizeof(char *) * 1024);
    int file_count = 0;
    
    while ((entry = readdir(dir)) != NULL) {
        // Skip hidden files if -a not specified
        if (!show_all && entry->d_name[0] == '.') {
            continue;
        }
        
        filenames[file_count] = malloc(strlen(entry->d_name) + 1);
        strcpy(filenames[file_count], entry->d_name);
        file_count++;
    }
    
    closedir(dir);
    
    // Sort filenames lexicographically
    qsort(filenames, file_count, sizeof(char *), compare_strings);
    
    // Print total if showing details
    if (show_details) {
        long total_blocks = 0;
        for (int i = 0; i < file_count; i++) {
            char filepath[8192];  // Increased buffer size
            // Check for potential overflow before formatting
            if (strlen(resolved_path) + strlen(filenames[i]) + 2 > sizeof(filepath)) {
                fprintf(stderr, "reveal: path too long for %s\n", filenames[i]);
                continue;
            }
            snprintf(filepath, sizeof(filepath), "%s/%s", resolved_path, filenames[i]);
            struct stat file_stat;
            if (stat(filepath, &file_stat) == 0) {
                total_blocks += file_stat.st_blocks;
            }
        }
        printf("total %ld\n", total_blocks / 2);
    }
    
    // Print files
    for (int i = 0; i < file_count; i++) {
        char filepath[8192];  // Increased buffer size
        // Check for potential overflow before formatting
        if (strlen(resolved_path) + strlen(filenames[i]) + 2 > sizeof(filepath)) {
            fprintf(stderr, "reveal: path too long for %s\n", filenames[i]);
            print_colored_name(filenames[i], filenames[i]);  // Just print filename
            printf("\n");
        } else {
            snprintf(filepath, sizeof(filepath), "%s/%s", resolved_path, filenames[i]);
            print_file_info(filepath, filenames[i], show_details);
        }
        free(filenames[i]);
    }
    
    free(filenames);
}