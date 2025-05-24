#include "log.h"

void load_log(char **log_data, int *log_count)
{
    FILE *fptr = fopen("logfile.txt", "r");
    if (fptr == NULL)
    {
        return;
    }
    char buffer[4096];
    *log_count = 0;
    while ((*log_count < 15) && (fgets(buffer, 4096, fptr) != NULL))
    {
        int n = strlen(buffer);
        if (buffer[n - 1] == '\n')
        {
            buffer[n - 1] = '\0';
        }
        strcpy(log_data[*log_count], buffer);
        (*log_count)++;
    }
    fclose(fptr);
}

void add_input(char *input, char **log_data, int *log_count)
{
    if ((*log_count) > 0)
    {
        if (strcmp(log_data[(*log_count) - 1], input) == 0)
        {
            return;
        }
    }
    if ((*log_count) < 15)
    {
        strcpy(log_data[*log_count], input);
        (*log_count)++;
    }
    else if ((*log_count) == 15)
    {
        for (int i = 1; i < 15; i++)
        {
            strcpy(log_data[i - 1], log_data[i]);
        }
        strcpy(log_data[(*log_count) - 1], input);
    }
    FILE *fptr = fopen("logfile.txt", "w");
    if (fptr == NULL)
    {
        return;
    }
    else if (fptr != NULL)
    {
        fclose(fptr);
    }
    fptr = fopen("logfile.txt", "w");
    if (fptr == NULL)
    {
        return;
    }
    for (int i = 0; i < (*log_count); i++)
    {
        fprintf(fptr, "%s\n", log_data[i]);
    }
    fclose(fptr);
}

void log_cmd(char **arguments, int narg, char **log_data, int *log_count)
{
    if (narg == 0)
    {
        FILE *fptr = fopen("logfile.txt", "r");
        if (fptr == NULL)
        {
            return;
        }
        char buffer[4096];
        while (fgets(buffer, 4096, fptr) != NULL)
        {
            // int n = strlen(buffer);
            // if (buffer[n - 1] == '\n')
            // {
            //     buffer[n - 1] = '\0';
            // }
            printf("%s",buffer);
        }
        fclose(fptr);
    }
    else if (strcmp(arguments[0], "purge") == 0)
    {
        FILE *fptr = fopen("logfile.txt", "w");
        if (fptr != NULL)
        {
            fclose(fptr);
        }
        *log_count=0;
    }
}