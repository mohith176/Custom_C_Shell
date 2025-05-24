#include "fgnbg.h"
int get_tpgid()
{
    int fd;
    pid_t pgid;

    // Open the terminal file, e.g., /dev/tty
    fd = open("/dev/tty", O_RDONLY);
    if (fd == -1) {
        perror("Failed to open terminal file");
        return 1;
    }

    // Get the process group ID of the terminal
    pgid = tcgetpgrp(fd);
    if (pgid == -1) {
        perror("tcgetpgrp failed");
        close(fd);
        return 1;
    }
    // printf("Process group ID of the terminal: %d\n", pgid);
   
    // Close the file descriptor
    close(fd);

    return pgid;
}
void fg_cmd(int pid)
{
    int del = 0;
    int status,tpgid=0;
    tpgid = get_tpgid();
    if(tpgid==1)
    {
        return;
    }
    for (int i = 0; i < bgcount; i++)
    {
        if (bgarray[i].bgpid == pid)
        {
            if (kill(bgarray[i].bgpid, SIGCONT) == -1)
            {
                printf(RED "signal not sent properly\n" RESET);
                return;
            }
            setpgid(pid,tpgid);
            waitpid(pid, &status, WUNTRACED);
            del = 1;
            break;
        }
    }
    if (del == 0)
    {
        printf(RED "No such process found\n" RESET);
        return;
    }
}

void bg_cmd(int pid)
{
    int del = 0;
    for (int i = 0; i < bgcount; i++)
    {
        if (bgarray[i].bgpid == pid)
        {
            if (kill(bgarray[i].bgpid, SIGCONT) == -1)
            {
                printf(RED "signal not sent properly\n" RESET);
            }
            del = 1;
            break;
        }
    }
    if (del == 0)
    {
        printf(RED "No such process found\n" RESET);
    }
}