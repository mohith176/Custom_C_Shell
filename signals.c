#include "signals.h"

void ping(int pid, int sig_no)
{
    sig_no = ((sig_no) % 32);
    if (kill(pid, sig_no) == -1)
    {
        printf(RED "signal not sent\n" RESET);
    }
    else
    {
        printf("Sent signal %d to process with pid %d\n", sig_no, pid);
    }
}

void handle_ctrl_c(int sig)
{   
    
    if(forepid ==-1)
    {
        return;
    }
    printf("\n");
    if(kill(forepid, SIGINT) == -1)
    {
        printf(RED "signal not sent\n" RESET);
    }
    forepid=-1;
}


void handle_ctrl_z(int sig)
{
    if(forepid ==-1)
    {
        return;
    }
    printf("\n");
    // setpgid(forepid,0);
    if(kill(forepid, SIGTSTP) == -1) 
    {
        printf(RED "signal not sent\n" RESET);
    }
    forepid=-1;
}


void handle_ctrl_D()
{
  for(int i=0;i<bgcount;i++)
  {
    kill(bgarray[i].bgpid,9);
  }
  exit(0);
}