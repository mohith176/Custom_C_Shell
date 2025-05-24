#include "display.h"

int forepid=-1;
void displayfn()
{
    struct passwd *pw;
    struct utsname sys_info;
    pw = getpwuid(getuid());
    if (pw == NULL)
    {
        perror("getpwuid\n");
        exit(1);
    }
    if (uname(&sys_info) == -1)
    {
        perror("uname\n");
        exit(1);
    }
    char *Username = pw->pw_name;
    char *Systemname = sys_info.nodename;
    char homedir[4096];
    char prevdir[4096];
    int n = sizeof(homedir);

    getcurdir(homedir, n);
    int homepathlength = strlen(homedir);
    char details[4096] = "";

    int flag = 0;
    int logflag = 0;
    char **log_data = (char **)malloc(sizeof(char *) * 15);
    for (int i = 0; i < 15; i++)
    {
        log_data[i] = (char *)malloc(sizeof(char) * 4096);
    }
    int log_count = 0;
    load_log(log_data, &log_count);
    signal(SIGCHLD, sigchildHandler);
    signal(SIGINT, handle_ctrl_c); 
    signal(SIGTSTP, handle_ctrl_z); 
    while (1)
    {
        char cwd[4096];
        char shellpath[4096] = "";
        getcurdir(cwd, 4096);
        if (strncmp(cwd, homedir, homepathlength) != 0)
        {
            flag = 1;
        }
        else
        {
            flag = 0;
        }
        if (flag == 0)
        {
            Setshellpath(homedir, cwd, shellpath);
        }
        if (flag == 0)
        {
            printf(GREEN "<%s@%s:~%s%s>" RESET, Username, Systemname, shellpath, details);
            strcpy(details, "");
        }
        else if (flag == 1)
        {
            printf(GREEN "<%s@%s:%s%s>" RESET, Username, Systemname, cwd, details);
            strcpy(details, "");
        }
        char s[4096];

        if (fgets(s, 4096, stdin) == NULL)
        {
            if (feof(stdin))
            {
                printf("\n");
                handle_ctrl_D(); 
            }
        }
        if (s[strlen(s) - 1] == '\n')
        {
            s[strlen(s) - 1] = '\0';
        }
        if (strcmp(s, "") == 0)
        {
            continue;
        }

        maketokens(s, homedir, prevdir, details, logflag, log_data, &log_count);
    }
}