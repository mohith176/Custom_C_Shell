#include "tokenize.h"

char *insert(int pos, char *str, char a)
{
  int len = strlen(str);
  for (int i = len; i >= pos; i--)
  {
    str[i + 1] = str[i];
  }
  str[pos] = a;
  return str;
}

int is_only_spaces(const char *str)
{
  while (*str)
  {
    if (!isspace((unsigned char)*str))
    {
      return 0; // Found a non-space character
    }
    str++;
  }
  return 1; // All characters are spaces
}

void maketokens(char *input, char *homedir, char *prevdir, char *details, int logflag, char **log_data, int *log_count)
{
  if (input == NULL)
  {
    return;
  }

    // Check if input contains pipes or redirection
    if (strchr(input, '|') != NULL || strchr(input, '>') != NULL || strchr(input, '<') != NULL) {
      pipe_info info;
      parse_redirection_and_pipes(input, &info);
      execute_with_redirection_and_pipes(&info, homedir, prevdir, details);
      
      if (logflag != 1) {
        add_input(input, log_data, log_count);
      }
      return;
    }
  char input_copy[4096];
  strcpy(input_copy, input);
  for (int i = 0; i < strlen(input); i++)
  {
    if (input[i] == '&')
    {
      input = insert(i + 1, input, ';');
    }
  }
  char *token;
  char *token1;
  char *saveptr;
  char *saveptr1;
  token = strtok_r(input, ";", &saveptr);
  // while(token!=NULL)
  // {
  //   char* token1;
  //   filterand(homedir,token);
  //   token=strtok(NULL,";");
  // }
  while (token != NULL)
  {
    if (is_only_spaces(token))
    {
      token = strtok_r(NULL, ";", &saveptr);
      continue;
    }
    // printf("%s\n",token);
    if (strchr(token, '&') == NULL)
    {
      // printf("foreground\n");
      char **arguments;
      arguments = (char **)malloc(sizeof(char *) * 4096);
      for (int i = 0; i < 4096; i++)
      {
        arguments[i] = (char *)malloc(sizeof(char) * 4096);
      }
      char command[4096];
      int narg = 0;
      int m = 0;
      token1 = strtok_r(token, " ", &saveptr1);
      while (token1 != NULL)
      {
        if (m == 0)
        {
          strcpy(command, token1);
          m = 1;
        }
        else
        {
          strcpy(arguments[narg], token1);
          narg++;
        }
        token1 = strtok_r(NULL, " ", &saveptr1);
      }
      if (strcmp(command, "hop") == 0)
      {
        hopcmd(homedir, arguments, narg, prevdir);
      }
      else if (strcmp(command, "proclore") == 0)
      {
        int pid;
        if (narg == 0)
        {
          pid = getpid();
          proclore_cmd(pid);
        }
        else
        {
          pid = atoi(arguments[0]);
          proclore_cmd(pid);
        }
      }
      else if (strcmp(command, "log") == 0)
      {
        logflag = 1;
        if (narg != 2)
        {
          log_cmd(arguments, narg, log_data, log_count);
        }
        else
        {
          int val = atoi(arguments[1]);
          if((val > (*log_count)) || (val < 1))
          {
            printf(RED "INVALID INDEX\n" RESET);
          }
          else
          {
           maketokens(log_data[(*log_count) - val], homedir, prevdir, details, logflag, log_data, log_count);
          }
        }
      }
      else if(strcmp(command, "iMan")==0)
      {
        if(narg < 1)
        {
          printf(RED "no arguments provided\n" RESET);
        }
        else
        {
          iman_cmd(arguments[0]);
        }
      }
      else if(strcmp(command,"neonate")==0)
      {
        if(narg < 2)
        {
          printf(RED "not enough arguments provided\n" RESET);
        }
        else
        {
          neonate_cmd(arguments[1]);
        }
      }
      else if(strcmp(command,"activities")==0)
      {
        activities_cmd();
      }
      else if(strcmp(command,"ping")==0)
      {
        if(narg < 2)
        {
          printf(RED "not enough arguments provided\n" RESET);
        }
        else
        {
          ping(atoi(arguments[0]),atoi(arguments[1]));
        }
      }
      else if(strcmp(command,"reveal")==0)
      {
        reveal_cmd(arguments, narg, homedir);
      }
      else if(strcmp(command,"seek")==0)
      {
        seek_cmd(arguments, narg, homedir);
      }
      else if(strcmp(command,"bg")==0)
      {
        if(narg == 1)
        {
          bg_cmd(atoi(arguments[0]));
        }
        else
        {
          printf(RED "enter correct no of arguments\n" RESET);
        }
      }
      else if(strcmp(command,"fg")==0)
      {
        if(narg==1)
        {
          fg_cmd(atoi(arguments[0]));
        }
        else
        {
         printf(RED "enter correct no of arguments\n" RESET);
        }
      }
      else
      {
        foreground(command, arguments, details, narg);
      }
    }
    else
    {
      // printf("background\n");
      char **arguments;
      arguments = (char **)malloc(sizeof(char *) * 4096);
      for (int i = 0; i < 4096; i++)
      {
        arguments[i] = (char *)malloc(sizeof(char) * 4096);
      }
      char command[4096];
      int narg = 0;
      int m = 0;
      token1 = strtok_r(token, " ", &saveptr1);
      while (token1 != NULL)
      {
        if (m == 0)
        {
          strcpy(command, token1);
          m = 1;
        }
        else
        {
          strcpy(arguments[narg], token1);
          narg++;
        }
        token1 = strtok_r(NULL, " ", &saveptr1);
      }
      if (strcmp(command, "log") == 0)
      {
        logflag = 1;
      }
      background(command, arguments, narg);
    }
    token = strtok_r(NULL, ";", &saveptr);
  }
  if (logflag != 1)
  {
    add_input(input_copy, log_data, log_count);
  }
}