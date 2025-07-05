#ifndef REDIRECTION_H
#define REDIRECTION_H
#include "common.h"
#include "tokenize.h"
typedef struct {
    char command[4096];
    char input_file[4096];
    char output_file[4096];
    int append_mode;  // 0 for >, 1 for >>
    int has_input_redirect;
    int has_output_redirect;
} redirect_info;

typedef struct {
    char commands[100][4096];  // Array to store individual commands
    int num_commands;
    redirect_info redirect;
} pipe_info;

void parse_redirection_and_pipes(char *input, pipe_info *info);
void execute_with_redirection_and_pipes(pipe_info *info, char *homedir, char *prevdir, char *details);
void execute_single_command_with_redirection(char *command, redirect_info *redirect, char *homedir, char *prevdir, char *details);

#endif