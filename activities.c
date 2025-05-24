#include "activities.h"

int compare(const void *a, const void *b)
{
    bgpro *bg_a = (bgpro *)a;
    bgpro *bg_b = (bgpro *)b;
    return (bg_a->bgpid - bg_b->bgpid);
}

void activities_cmd()
{
    bgpro bgtemp[bgcount];

    for (int i = 0; i < bgcount; i++)
    {
        strcpy(bgtemp[i].bgcmd, bgarray[i].bgcmd);
        bgtemp[i].bgpid = bgarray[i].bgpid;
    }

    qsort(bgtemp, bgcount, sizeof(bgpro), compare);

    for (int i = 0; i < bgcount; i++)
    {
        char path[4096];
        FILE *fptr;
        snprintf(path, sizeof(path), "/proc/%d/stat", bgtemp[i].bgpid);
        fptr = fopen(path, "r");
        if (fptr != NULL)
        {
            char status;
            fscanf(fptr, "%*d %*s %c", &status);
            fclose(fptr);
            if (status != 'T')
            {
                printf("%d : %s - Running\n", bgtemp[i].bgpid, bgtemp[i].bgcmd);
            }
            else
            {
                printf("%d : %s - Stopped\n", bgtemp[i].bgpid, bgtemp[i].bgcmd);
            }
        }
    }
}