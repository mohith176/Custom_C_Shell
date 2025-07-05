# Custom C Shell Implementation

A comprehensive shell implementation in C that provides various built-in commands, I/O redirection, pipes, and advanced system process management capabilities.

## Features

### Built-in Commands

- **hop** - Change directory with support for relative/absolute paths, `~`, `-`, `.`, `..`
- **reveal** - List files and directories with support for `-a` (show hidden files) and `-l` (detailed view) flags
- **seek** - Search for files/directories recursively with flags `-d` (directories only), `-f` (files only), and `-e` (execute/navigate)
- **proclore** - Display process information (PID, status, process group, virtual memory, executable path)
- **log** - Command history management with purge functionality (stores up to 15 commands)
- **iMan** - Fetch and display manual pages from man.he.net
- **neonate** - Display most recent process ID at specified intervals (press 'x' to exit)
- **activities** - List all background processes with their status (Running/Stopped)
- **ping** - Send signals to processes by PID
- **fg** - Bring background process to foreground
- **bg** - Continue stopped background process

### System Features

- **Foreground/Background Process Execution** - Run commands in foreground or background (using `&`)
- **I/O Redirection** - Support for `>` (output), `>>` (append), `<` (input) redirection
- **Pipes** - Support for multiple pipes to chain commands together
- **Combined Redirection and Pipes** - Full support for using redirection with pipes
- **Signal Handling** - Proper handling of Ctrl+C (SIGINT), Ctrl+Z (SIGTSTP), and Ctrl+D (EOF)
- **Process Monitoring** - Track background processes and their status changes
- **Command History** - Store and retrieve up to 15 recent commands with indexing
- **Shell Prompt** - Dynamic prompt showing username, hostname, current directory, and command execution time
- **Color Support** - Color-coded output for files (white), directories (blue), and executables (green)

## Compilation and Usage

### Compilation
```bash
make main

or

gcc main.c tokenize.c display.c hop.c syscmd.c proclore.c log.c iMan.c neonate.c activities.c signals.c fgnbg.c reveal.c seek.c redirection.c



Running
./a.out

```
## Command Examples

### Basic Navigation

```bash
hop ~/Documents          # Change to Documents directory
hop -                    # Go to previous directory
reveal -la              # List all files with details
seek -f test.txt        # Search for files starting with "test.txt"
```

### Process Management

```bash
sleep 100 &             # Run command in background
activities              # List background processes
fg 1234                 # Bring process 1234 to foreground
ping 1234 9             # Send SIGKILL to process 1234
```

### I/O Redirection and Pipes

```bash
echo "Hello World" > file.txt     # Redirect output to file
cat < file.txt                    # Read input from file
echo "New line" >> file.txt       # Append to file
cat file.txt | wc | sort          # Chain commands with pipes
cat < input.txt | grep "pattern" > output.txt  # Combined redirection and pipes
```

### Command History

```bash
log                     # Display command history
log execute 3           # Execute 3rd most recent command
log purge               # Clear command history
```

### Advanced Features

```bash
neonate -n 2            # Show newest PID every 2 seconds
seek -e -f config       # Find and open config file if unique match
iMan ls                 # Display manual page for ls command
proclore 1234           # Show detailed process information
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

