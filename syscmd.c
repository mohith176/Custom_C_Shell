#include "syscmd.h"

// char bgcmd[4096][4096];
// int bgpid[4096];
int bgcount = 0;
// typedef struct
// {
//     char bgcmd[4096];
//     int bgpid;
// }bgpro;
bgpro bgarray[4096];

void sigchildHandler(int sig)
{
    int status, rc_wait;
    while ((rc_wait = waitpid(-1, &status, WNOHANG)) > 0)
    {
        for (int i = 0; i < bgcount; i++)
        {
            if (rc_wait == bgarray[i].bgpid)
            {
                if (WIFEXITED(status))
                {
                    printf("\n%s exited normally (%d)\n", bgarray[i].bgcmd, bgarray[i].bgpid);
                }
                else if (WIFSIGNALED(status))
                {
                    printf("\n%s exited abnormally (%d)\n", bgarray[i].bgcmd, bgarray[i].bgpid);
                }
                for (int j = i; j < (bgcount - 1); j++)
                {
                    // strcpy(bgcmd[j], bgcmd[j + 1]);
                    // bgpid[j] = bgpid[j + 1];
                    bgarray[j] = bgarray[j + 1];
                }
                bgcount--;
                break;
            }
        }
    }
}

void foreground(char *command, char **arguments, char *details, int narg)
{
    char **args = (char **)malloc(sizeof(char *) * (narg + 2));
    for (int i = 0; i < (narg + 2); i++)
    {
        args[i] = (char *)malloc(sizeof(char) * 4096);
    }
    int rc = fork();
    if (rc == -1)
    {
        perror("fork failed\n");
        exit(1);
    }
    else if (rc == 0)
    {
        strcpy(args[0], command);
        for (int i = 0; i < narg; i++)
        {
            strcpy(args[i + 1], arguments[i]);
        }
        args[narg + 1] = NULL;
        if (execvp(command, args) == -1)
        {
            printf(RED "exec failed\n" RESET);
            exit(1);
        }
    }
    else
    {
        int status;
        forepid = rc;
        time_t stime = time(NULL);
        setpgid(rc,0);
        int rc_wait = waitpid(forepid, &status, WUNTRACED);
        time_t etime = time(NULL);
        int dur = (round)(etime - stime);
        // printf("%d\n",dur);
        // printf("mohith\n");
        char buffer[4096];
        if (dur > 2)
        {
            sprintf(details, " %s : %ds", command, dur);
            //  printf("details=>%s\n",details);
        }
        if (WIFSTOPPED(status))
        {  
            strcpy(bgarray[bgcount].bgcmd,command);
            bgarray[bgcount].bgpid=rc;
            bgcount++;
        }
    }
    for (int i = 0; i < (narg + 2); i++)
    {
        free(args[i]);
    }
    free(args);
}

void background(char *command, char **arguments, int narg)
{
    char **args = (char **)malloc(sizeof(char *) * (narg + 1));
    for (int i = 0; i < (narg + 1); i++)
    {
        args[i] = (char *)malloc(sizeof(char) * 4096);
    }
    strcpy(args[0], command);
    for (int i = 0; i < narg; i++)
    {
        strcpy(args[i + 1], arguments[i]);
    }
    args[narg] = NULL;
    int rc = fork();
    if (rc == -1)
    {
        perror("fork failed\n");
        exit(1);
    }
    else if (rc == 0)
    {
        setpgid(0,0);
        if (execvp(command, args) == -1)
        {
            printf(RED "exec failed\n" RESET);
            exit(1);
        }
    }
    else
    {
        printf("%d\n", rc);
        strcpy(bgarray[bgcount].bgcmd, command);
        bgarray[bgcount].bgpid = rc;
        bgcount++;
    }
    for (int i = 0; i < (narg + 1); i++)
    {
        free(args[i]);
    }
    free(args);
}