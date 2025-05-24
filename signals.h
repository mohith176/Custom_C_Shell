#ifndef SIGNALS_H
#define SIGNALS_H
#include "common.h"
#include "syscmd.h"
void ping(int pid,int sig_no);
void handle_ctrl_c(int sig);
void handle_ctrl_z(int sig);
void handle_ctrl_D();
#endif