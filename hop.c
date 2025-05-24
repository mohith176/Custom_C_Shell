#include "hop.h"
void getcurdir(char* cwd,int n)
{
      if (getcwd(cwd,n) == NULL) 
      {
        perror("getcwd");
        exit(1);
      } 
}

void change_directory(char *path) 
{
    if (chdir(path) != 0) 
    {
        perror("hop");
        return;
    }
    char cwd[4096];
    if (getcwd(cwd, sizeof(cwd)) != NULL) 
    {
        printf("%s\n", cwd);
    } 
    else 
    {
        perror("getcwd");
    }
}

void hopmtds(char* prevdir,char* homedir,char* argu)
{
  if((strcmp(argu,"")==0) ||(strcmp(argu,"~")==0))
  {
    change_directory(homedir);
  }
  else if(strcmp(argu,"-")==0)
  {
    if (prevdir[0] != '\0')  
    {
        change_directory(prevdir);
    } 
    else 
    {
        fprintf(stderr, "hop: OLDPWD not set\n");
    }
  }
  else if(strcmp(argu,".")==0)
  {
    char cur[4096];
    if (getcwd(cur,4096) == NULL) 
    {
       perror("getcwd");
       exit(1);
    }
    printf("%s\n",cur);
  }
  else if(strcmp(argu,"..")==0)
  {
    change_directory("..");
  }
  else
  {
    if(argu[0]=='/')
    {
       change_directory(argu);
    }
    else if(argu[0]=='~')
    {
        char str[4096];
        strcpy(str,homedir);
        int n=strlen(homedir);
        for(int i=1;i<=(strlen(argu));i++)
        {
            str[n+i-1]=argu[i];
        }
        change_directory(str);
    }
    else
    {
        char cur[4096];
        if (getcwd(cur,4096) == NULL) 
        {
            perror("getcwd");
            exit(1);
        }

        strcat(cur,"/");
        strcat(cur,argu);

        change_directory(cur);
    }
  }
}


void hopcmd(char* homedir,char** arguments,int narg,char* prevdir)
{
    if(narg==0)
    {
        getcurdir(prevdir,4096);
        hopmtds(prevdir,homedir,"");
    }
    for(int i=0;i<narg;i++)
    {
       if(strcmp(arguments[i],"-")!=0)
       {
        getcurdir(prevdir,4096);
        hopmtds(prevdir,homedir,arguments[i]);
       }
       else
       {
        char temp[4096];
        getcurdir(temp,4096);
        hopmtds(prevdir,homedir,arguments[i]);
        strcpy(prevdir,temp);
       }
    }
}

void Setshellpath(char* homedir,char* cwd,char* shellpath)
{
    int homelength=strlen(homedir);
    int j=0;
    for(int i=homelength;i<=strlen(cwd);i++)
    {
      shellpath[j]=cwd[i];
      j++;
    }
}

