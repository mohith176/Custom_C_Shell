#ifndef IMAN_H
#define IMAN_H
#include "common.h"
void send_request(int socket_fd,char* argument);
void filter(int socket_fd);
void iman_cmd(char* argument);
#endif