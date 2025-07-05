#ifndef REVEAL_H
#define REVEAL_H
#include "common.h"
#include <sys/stat.h>
#include <grp.h>
#include <pwd.h>
#include <time.h>

void reveal_cmd(char **arguments, int narg, char *homedir);
void print_file_info(const char *filepath, const char *filename, int show_details);
void print_colored_name(const char *filepath, const char *filename);
int compare_strings(const void *a, const void *b);
void resolve_path(const char *path, const char *homedir, char *resolved_path);

#endif