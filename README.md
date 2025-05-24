# Shell Implementation (Mini Project 1)

A custom shell implementation in C that provides various built-in commands and system process management capabilities.

## Features

### Built-in Commands

- **hop** - Change directory with support for relative/absolute paths, `~`, `-`, `.`, `..`
- **proclore** - Display process information (PID, status, process group, virtual memory, executable path)
- **log** - Command history management with purge functionality
- **iMan** - Fetch and display manual pages from man.he.net
- **neonate** - Display most recent process ID at specified intervals
- **activities** - List all background processes with their status
- **ping** - Send signals to processes
- **fg** - Bring background process to foreground
- **bg** - Continue stopped background process

### System Features

- **Foreground/Background Process Execution** - Run commands in foreground or background (using `&`)
- **Signal Handling** - Proper handling of Ctrl+C, Ctrl+Z, and Ctrl+D
- **Process Monitoring** - Track background processes and their status
- **Command History** - Store and retrieve up to 15 recent commands
- **Shell Prompt** - Dynamic prompt showing username, hostname, and current directory

## Compilation and Usage

### Compilation
```bash
make main

or

gcc main.c tokenize.c display.c hop.c syscmd.c proclore.c log.c iMan.c neonate.c activities.c signals.c fgnbg.c -o shell

Running
./a.out

```


## Assumptions
1) I am assuming a maximum input of 4096 characters
2) I am also printing errorneous commands in my log file
3) if log is not give as command for input,then i am also storing it
(for ex : in a input like hop log in this input log is not a command but may be a directory so i am storing this  but in case somewhere in input log is given as command then i am not storing such inputs in my logfile)
4) i am assuming i can run at max of 4096 background processes
5) i am using a struct array to store info about my background processes
6) while printing activities i am considering in the order of pid numbers
7) i am only printing two states for activities
(i.e running and stopped)
8) In iMan i am printing html tags but also making sure not to print headers

