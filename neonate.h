#ifndef NEONATE_H
#define NEONATE_H
#include "common.h"
int get_recentpid();
int check_x();
void neonate_cmd(char* argument);
void block_signals();
void restore_signals();
#endif