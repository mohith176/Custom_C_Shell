#include "seek.h"

void resolve_seek_path(const char *path, const char *homedir, char *resolved_path) {
    if (strcmp(path, "~") == 0) {
        strcpy(resolved_path, homedir);
    } else if (strcmp(path, ".") == 0) {
        getcwd(resolved_path, 4096);
    } else if (strcmp(path, "..") == 0) {
        getcwd(resolved_path, 4096);
        strcat(resolved_path, "/..");
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

int has_read_permission(const char *filepath) {
    return access(filepath, R_OK) == 0;
}

int has_execute_permission(const char *dirpath) {
    return access(dirpath, X_OK) == 0;
}

void search_recursive(const char *search_dir, const char *target, const char *base_dir, 
                     int only_dirs, int only_files, int *match_count, 
                     char matches[][4096], int *match_types) {
    DIR *dir;
    struct dirent *entry;
    struct stat file_stat;
    char full_path[8192];  // Increased buffer size
    char relative_path[4096];
    
    dir = opendir(search_dir);
    if (dir == NULL) {
        return;
    }
    
    while ((entry = readdir(dir)) != NULL) {
        // Skip . and ..
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        
        // Check for potential overflow before formatting
        if (strlen(search_dir) + strlen(entry->d_name) + 2 > sizeof(full_path)) {
            fprintf(stderr, "seek: path too long for %s\n", entry->d_name);
            continue;
        }
        
        // Create full path
        snprintf(full_path, sizeof(full_path), "%s/%s", search_dir, entry->d_name);
        
        if (stat(full_path, &file_stat) == -1) {
            continue;
        }
        
        // Check if name matches (exact match or starts with target)
        if (strncmp(entry->d_name, target, strlen(target)) == 0) {
            int is_dir = S_ISDIR(file_stat.st_mode);
            
            // Apply filters
            if (only_dirs && !is_dir) {
                // Skip files when only looking for directories
            } else if (only_files && is_dir) {
                // Skip directories when only looking for files
            } else {
                // Create relative path from base directory
                const char *rel_start = search_dir + strlen(base_dir);
                if (rel_start[0] == '/') rel_start++;
                
                if (strlen(rel_start) == 0) {
                    snprintf(relative_path, sizeof(relative_path), "./%s", entry->d_name);
                } else {
                    snprintf(relative_path, sizeof(relative_path), "./%s/%s", rel_start, entry->d_name);
                }
                
                // Store match
                if (*match_count < 1000) {
                    strcpy(matches[*match_count], relative_path);
                    match_types[*match_count] = is_dir ? 1 : 0; // 1 for dir, 0 for file
                    (*match_count)++;
                }
            }
        }
        
        // Recursively search subdirectories
        if (S_ISDIR(file_stat.st_mode)) {
            search_recursive(full_path, target, base_dir, only_dirs, only_files, 
                           match_count, matches, match_types);
        }
    }
    
    closedir(dir);
}

void seek_cmd(char **arguments, int narg, char *homedir) {
    int only_dirs = 0;
    int only_files = 0;
    int execute_flag = 0;
    char target[4096];
    char search_path[4096] = ".";
    int target_specified = 0;
    int path_specified = 0;
    
    // Parse arguments
    for (int i = 0; i < narg; i++) {
        if (arguments[i][0] == '-') {
            // Parse flags
            for (int j = 1; j < strlen(arguments[i]); j++) {
                if (arguments[i][j] == 'd') {
                    only_dirs = 1;
                } else if (arguments[i][j] == 'f') {
                    only_files = 1;
                } else if (arguments[i][j] == 'e') {
                    execute_flag = 1;
                }
            }
        } else {
            // First non-flag argument is target, second is path
            if (!target_specified) {
                strcpy(target, arguments[i]);
                target_specified = 1;
            } else if (!path_specified) {
                strcpy(search_path, arguments[i]);
                path_specified = 1;
            }
        }
    }
    
    // Check for conflicting flags
    if (only_dirs && only_files) {
        printf("Invalid flags!\n");
        return;
    }
    
    // Check if target is specified
    if (!target_specified) {
        printf("No target specified!\n");
        return;
    }
    
    // Resolve search path
    char resolved_path[4096];
    resolve_seek_path(search_path, homedir, resolved_path);
    
    // Check if search directory exists
    struct stat dir_stat;
    if (stat(resolved_path, &dir_stat) == -1 || !S_ISDIR(dir_stat.st_mode)) {
        printf("Invalid directory!\n");
        return;
    }
    
    // Search for matches
    char matches[1000][4096];
    int match_types[1000]; // 0 for file, 1 for directory
    int match_count = 0;
    
    search_recursive(resolved_path, target, resolved_path, only_dirs, only_files, 
                    &match_count, matches, match_types);
    
    if (match_count == 0) {
        printf("No match found!\n");
        return;
    }
    
    // Handle -e flag
    if (execute_flag && match_count == 1) {
        char full_path[8192];  // Increased buffer size
        const char *match_path = matches[0] + 2; // Skip "./"
        
        // Check for potential overflow before formatting
        if (strlen(resolved_path) + strlen(match_path) + 2 > sizeof(full_path)) {
            printf("Missing permissions for task!\n");
            return;
        }
        
        snprintf(full_path, sizeof(full_path), "%s/%s", resolved_path, match_path);
        
        if (match_types[0] == 1) { // Directory
            if (!has_execute_permission(full_path)) {
                printf("Missing permissions for task!\n");
                return;
            }
            // Change to the directory
            printf("%s\n", matches[0]);
            if (chdir(full_path) == 0) {
                char new_cwd[4096];
                if (getcwd(new_cwd, sizeof(new_cwd)) != NULL) {
                    // Directory changed successfully
                }
            }
        } else { // File
            if (!has_read_permission(full_path)) {
                printf("Missing permissions for task!\n");
                return;
            }
            // Print file content
            printf("%s\n", matches[0]);
            FILE *file = fopen(full_path, "r");
            if (file != NULL) {
                char buffer[4096];
                while (fgets(buffer, sizeof(buffer), file) != NULL) {
                    printf("%s", buffer);
                }
                fclose(file);
            }
        }
        return;
    }
    
    // Print all matches with colors
    for (int i = 0; i < match_count; i++) {
        if (match_types[i] == 1) { // Directory
            printf(BLUE "%s" RESET "\n", matches[i]);
        } else { // File
            printf(GREEN "%s" RESET "\n", matches[i]);
        }
    }
}