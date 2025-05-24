#include "proclore.h"
void proclore_cmd(int pid)
{
    char path[4096];
    FILE *status_file;
    printf("pid : %d\n", pid);

    snprintf(path, sizeof(path), "/proc/%d/stat", pid);
    status_file = fopen(path, "r");
    if (status_file)
    {
        int pgid;
        char status;
        fscanf(status_file, "%*d %*s %c %*d %d", &status, &pgid);
        fclose(status_file);
        if(pgid!=pid)
        {
            printf("process status : %c\n", status);
        }
        else
        {
            printf("process status : %c+\n", status);
        }
        printf("Process group : %d\n", pgid);
    }

    char buffer[4096];
    snprintf(path, sizeof(path), "/proc/%d/status", pid);
    status_file = fopen(path, "r");
    if (status_file)
    {
        while (fgets(buffer, sizeof(buffer), status_file))
        {
            if (strncmp(buffer, "VmSize:", 7) == 0)
            {
                printf("Virtual memory : %s", buffer + 12);
                break;
            }
        }
        fclose(status_file);
    }

    snprintf(path, sizeof(path), "/proc/%d/exe", pid);
    ssize_t len = readlink(path, buffer, sizeof(buffer) - 1);
    if (len != -1)
    {
        buffer[len] = '\0';
        printf("Executable path : %s\n", buffer);
    }
}