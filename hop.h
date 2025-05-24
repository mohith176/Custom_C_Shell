#ifndef HOP_H
#define HOP_H
#include "common.h"
void getcurdir(char* cwd,int n);
void hopcmd(char* homedir,char** arguments,int narg,char* prevdir);
void hopmtds(char* prevdir,char* homedir,char* cmd);
void change_directory(char *path);
void Setshellpath(char* homedir,char* cwd,char* shellpath);

#endif