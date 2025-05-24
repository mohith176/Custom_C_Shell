#ifndef TOKENIZE_H
#define TOKENIZE_H
#include"common.h"
#include "hop.h"
#include "syscmd.h"
#include "proclore.h"
#include "log.h"
#include "iMan.h"
#include "neonate.h"
#include "activities.h"
#include "signals.h"
#include "fgnbg.h"
void maketokens(char* input,char* homedir,char* prevdir,char* details,int logflag,char **log_data, int *log_count);
char* insert(int pos,char* str,char a);
int is_only_spaces(const char *str);
#endif
