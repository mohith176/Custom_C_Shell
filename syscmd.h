#ifndef SYSCMD_H
#define SYSCMD_H
#include "common.h"
extern int bgcount;
typedef struct
{
    char bgcmd[4096];
    int bgpid;
}bgpro;
extern bgpro bgarray[4096];
void sigchildHandler(int sig);
void foreground(char* command,char** arguments,char* details,int narg);
void background(char* command,char** arguments,int narg);
#endif