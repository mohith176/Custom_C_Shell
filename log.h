#ifndef LOG_H
#define LOG_H
#include "common.h"
void load_log(char** log_data,int* log_count);
void log_cmd(char** arguments,int narg,char **log_data, int *log_count);
void add_input(char* input,char** log_data,int* log_count);
#endif