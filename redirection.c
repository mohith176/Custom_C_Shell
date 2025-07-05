#include "redirection.h"
#include "tokenize.h"

void parse_redirection_and_pipes(char *input, pipe_info *info) {
    info->num_commands = 0;
    info->redirect.has_input_redirect = 0;
    info->redirect.has_output_redirect = 0;
    info->redirect.append_mode = 0;
    strcpy(info->redirect.input_file, "");
    strcpy(info->redirect.output_file, "");
    
    char *input_copy = strdup(input);
    char *token;
    char *saveptr;
    
    // Split by pipes first
    token = strtok_r(input_copy, "|", &saveptr);
    while (token != NULL && info->num_commands < 100) {
        // Trim leading/trailing spaces
        while (*token == ' ') token++;
        int len = strlen(token);
        while (len > 0 && token[len-1] == ' ') {
            token[len-1] = '\0';
            len--;
        }
        
        if (strlen(token) == 0) {
            printf("Invalid use of pipe\n");
            free(input_copy);
            return;
        }
        
        strcpy(info->commands[info->num_commands], token);
        info->num_commands++;
        token = strtok_r(NULL, "|", &saveptr);
    }
    
    if (info->num_commands == 0) {
        printf("Invalid use of pipe\n");
        free(input_copy);
        return;
    }
    
    // Parse redirection in first and last commands
    char *first_cmd = info->commands[0];
    char *last_cmd = info->commands[info->num_commands - 1];
    
    // Parse input redirection from first command
    char *input_redirect = strchr(first_cmd, '<');
    if (input_redirect != NULL) {
        *input_redirect = '\0';
        input_redirect++;
        while (*input_redirect == ' ') input_redirect++;
        
        char *space = strchr(input_redirect, ' ');
        if (space != NULL) *space = '\0';
        
        strcpy(info->redirect.input_file, input_redirect);
        info->redirect.has_input_redirect = 1;
        
        // Update first command without redirection
        int len = strlen(first_cmd);
        while (len > 0 && first_cmd[len-1] == ' ') {
            first_cmd[len-1] = '\0';
            len--;
        }
    }
    
    // Parse output redirection from last command
    char *output_redirect = strstr(last_cmd, ">>");
    if (output_redirect != NULL) {
        info->redirect.append_mode = 1;
        *output_redirect = '\0';
        output_redirect += 2;
    } else {
        output_redirect = strchr(last_cmd, '>');
        if (output_redirect != NULL) {
            info->redirect.append_mode = 0;
            *output_redirect = '\0';
            output_redirect++;
        }
    }
    
    if (output_redirect != NULL) {
        while (*output_redirect == ' ') output_redirect++;
        
        char *space = strchr(output_redirect, ' ');
        if (space != NULL) *space = '\0';
        
        strcpy(info->redirect.output_file, output_redirect);
        info->redirect.has_output_redirect = 1;
        
        // Update last command without redirection
        int len = strlen(last_cmd);
        while (len > 0 && last_cmd[len-1] == ' ') {
            last_cmd[len-1] = '\0';
            len--;
        }
    }
    
    free(input_copy);
}

void execute_single_command_with_redirection(char *command, redirect_info *redirect, char *homedir, char *prevdir, char *details) {
    int input_fd = STDIN_FILENO;
    int output_fd = STDOUT_FILENO;
    int saved_stdin = -1, saved_stdout = -1;
    
    // Handle input redirection
    if (redirect->has_input_redirect) {
        input_fd = open(redirect->input_file, O_RDONLY);
        if (input_fd == -1) {
            printf("No such input file found!\n");
            return;
        }
        saved_stdin = dup(STDIN_FILENO);
        dup2(input_fd, STDIN_FILENO);
    }
    
    // Handle output redirection
    if (redirect->has_output_redirect) {
        if (redirect->append_mode) {
            output_fd = open(redirect->output_file, O_WRONLY | O_CREAT | O_APPEND, 0644);
        } else {
            output_fd = open(redirect->output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        }
        
        if (output_fd == -1) {
            perror("open");
            if (redirect->has_input_redirect) {
                close(input_fd);
                dup2(saved_stdin, STDIN_FILENO);
                close(saved_stdin);
            }
            return;
        }
        
        saved_stdout = dup(STDOUT_FILENO);
        dup2(output_fd, STDOUT_FILENO);
    }
    
    // Execute the command
    char input_copy[4096];
    strcpy(input_copy, command);
    
    // Remove redirection logic from tokenize and execute directly
    char **arguments = (char **)malloc(sizeof(char *) * 4096);
    for (int i = 0; i < 4096; i++) {
        arguments[i] = (char *)malloc(sizeof(char) * 4096);
    }
    
    char cmd[4096];
    int narg = 0;
    int m = 0;
    
    char *token1;
    char *saveptr1;
    token1 = strtok_r(input_copy, " ", &saveptr1);
    while (token1 != NULL) {
        if (m == 0) {
            strcpy(cmd, token1);
            m = 1;
        } else {
            strcpy(arguments[narg], token1);
            narg++;
        }
        token1 = strtok_r(NULL, " ", &saveptr1);
    }
    
    // Execute built-in commands
    if (strcmp(cmd, "hop") == 0) {
        hopcmd(homedir, arguments, narg, prevdir);
    }
    else if (strcmp(cmd, "reveal") == 0) {
        reveal_cmd(arguments, narg, homedir);
    }
    else if (strcmp(cmd, "seek") == 0) {
        seek_cmd(arguments, narg, homedir);
    }
    else if (strcmp(cmd, "proclore") == 0) {
        int pid = (narg == 0) ? getpid() : atoi(arguments[0]);
        proclore_cmd(pid);
    }
    else if (strcmp(cmd, "activities") == 0) {
        activities_cmd();
    }
    else {
        // Execute external command
        foreground(cmd, arguments, details, narg);
    }
    
    // Restore file descriptors
    if (redirect->has_input_redirect) {
        close(input_fd);
        dup2(saved_stdin, STDIN_FILENO);
        close(saved_stdin);
    }
    
    if (redirect->has_output_redirect) {
        close(output_fd);
        dup2(saved_stdout, STDOUT_FILENO);
        close(saved_stdout);
    }
    
    // Free memory
    for (int i = 0; i < 4096; i++) {
        free(arguments[i]);
    }
    free(arguments);
}

void execute_with_redirection_and_pipes(pipe_info *info, char *homedir, char *prevdir, char *details) {
    if (info->num_commands == 1) {
        // Single command with possible redirection
        execute_single_command_with_redirection(info->commands[0], &info->redirect, homedir, prevdir, details);
        return;
    }
    
    // Multiple commands with pipes
    int pipes[info->num_commands - 1][2];
    
    // Create all pipes
    for (int i = 0; i < info->num_commands - 1; i++) {
        if (pipe(pipes[i]) == -1) {
            perror("pipe");
            return;
        }
    }
    
    for (int i = 0; i < info->num_commands; i++) {
        pid_t pid = fork();
        
        if (pid == 0) {
            // Child process
            
            // Handle input
            if (i == 0) {
                // First command - check for input redirection
                if (info->redirect.has_input_redirect) {
                    int input_fd = open(info->redirect.input_file, O_RDONLY);
                    if (input_fd == -1) {
                        printf("No such input file found!\n");
                        exit(1);
                    }
                    dup2(input_fd, STDIN_FILENO);
                    close(input_fd);
                }
            } else {
                // Not first command - read from previous pipe
                dup2(pipes[i-1][0], STDIN_FILENO);
            }
            
            // Handle output
            if (i == info->num_commands - 1) {
                // Last command - check for output redirection
                if (info->redirect.has_output_redirect) {
                    int output_fd;
                    if (info->redirect.append_mode) {
                        output_fd = open(info->redirect.output_file, O_WRONLY | O_CREAT | O_APPEND, 0644);
                    } else {
                        output_fd = open(info->redirect.output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                    }
                    
                    if (output_fd == -1) {
                        perror("open");
                        exit(1);
                    }
                    dup2(output_fd, STDOUT_FILENO);
                    close(output_fd);
                }
            } else {
                // Not last command - write to next pipe
                dup2(pipes[i][1], STDOUT_FILENO);
            }
            
            // Close all pipe file descriptors
            for (int j = 0; j < info->num_commands - 1; j++) {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }
            
            // Parse and execute command
            char **arguments = (char **)malloc(sizeof(char *) * 4096);
            for (int k = 0; k < 4096; k++) {
                arguments[k] = (char *)malloc(sizeof(char) * 4096);
            }
            
            char cmd[4096];
            int narg = 0;
            int m = 0;
            
            char *token1;
            char *saveptr1;
            char *input_copy = strdup(info->commands[i]);
            token1 = strtok_r(input_copy, " ", &saveptr1);
            while (token1 != NULL) {
                if (m == 0) {
                    strcpy(cmd, token1);
                    m = 1;
                } else {
                    strcpy(arguments[narg], token1);
                    narg++;
                }
                token1 = strtok_r(NULL, " ", &saveptr1);
            }
            
            // Execute command
            char **args = (char **)malloc(sizeof(char *) * (narg + 2));
            args[0] = strdup(cmd);
            for (int k = 0; k < narg; k++) {
                args[k + 1] = strdup(arguments[k]);
            }
            args[narg + 1] = NULL;
            
            if (execvp(cmd, args) == -1) {
                perror("execvp");
                exit(1);
            }
        }
    }
    
    // Parent process - close all pipes and wait for children
    for (int i = 0; i < info->num_commands - 1; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }
    
    for (int i = 0; i < info->num_commands; i++) {
        wait(NULL);
    }
}