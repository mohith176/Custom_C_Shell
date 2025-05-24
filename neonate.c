#include "neonate.h"


void block_signals() {
    signal(SIGINT, SIG_IGN);   
    signal(SIGTSTP, SIG_IGN);  
}


void restore_signals() {
    signal(SIGINT, SIG_DFL);   
    signal(SIGTSTP, SIG_DFL); 
}

int get_recentpid()
{
    DIR *proc_dir;
    struct dirent *entry;
    int recent_pid = 0;

    proc_dir = opendir("/proc");
    if (proc_dir == NULL)
    {
        perror("opendir");
        exit(EXIT_FAILURE);
    }

    while ((entry = readdir(proc_dir)) != NULL)
    {
        if (isdigit(*entry->d_name)) 
        {
            int pid = atoi(entry->d_name);
            if (pid > recent_pid)
            {
                recent_pid = pid; 
            }
        }
    }

    closedir(proc_dir);
    return recent_pid;
}

int check_x()
{
    struct termios oldt, newt;
    int ch;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldt);            
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);          
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);   
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK); 

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);   
    fcntl(STDIN_FILENO, F_SETFL, oldf);        

    if (ch == 'x')
    {
        return 1;
    }

    return 0;
}

void neonate_cmd(char* argument)
{
   int interval=atoi(argument);
   block_signals();
   if(interval < 1)
   {
     printf(RED "invalid time interval\n" RESET);
     exit(1);
   }
    while(1)
    {
        int recent_pid=get_recentpid();
        printf("%d\n",recent_pid);
        for(int i=0;i<interval;i++)
        {
            if(check_x())
            {
               restore_signals();
               return;
            }
            sleep(1);
        }
    }
}