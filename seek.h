#ifndef SEEK_H
#define SEEK_H
#include "common.h"
#include <sys/stat.h>

void seek_cmd(char **arguments, int narg, char *homedir);
void search_recursive(const char *search_dir, const char *target, const char *base_dir, 
                     int only_dirs, int only_files, int *match_count, 
                     char matches[][4096], int *match_types);
void resolve_seek_path(const char *path, const char *homedir, char *resolved_path);
int has_read_permission(const char *filepath);
int has_execute_permission(const char *dirpath);

#endif