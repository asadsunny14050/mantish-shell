# 🦗 Mantish 

A Unix shell implementation in C built for educational purposes, demonstrating systems programming concepts including process management, I/O redirection, and job control.

## Features

### Core Shell Operations
- Command execution with fork/exec
- Built-in commands: `cd`, `pwd`, `exit`, `history`, `jobs`, `fg`
- Command parsing with quote handling
- Background job execution with `&`

### I/O Operations
- File redirection: `>`, `<`, `>>`
- Error redirection: `2>`
- Pipeline support: `|`

### Command Control
- Conditional execution: `&&`, `||`
- Background process management
- Job control with foreground restoration

### Enhanced Features
- Command history with circular buffer storage
- Custom prompt with git branch display
- Directory path shortening (⾕ for home directory)

## Usage

### Building

#### Prerequisites

GCC compiler
GNU Make
Unix-like system (Linux, macOS, WSL)

```bash
make
./mantish
```

### Built-in Commands
- `cd [directory]` - Change directory (supports `..` and `-`)
- `pwd` - Print current directory
- `exit` - Exit shell
- `history` - Display command history
- `jobs` - List background jobs
- `fg [job_id]` - Bring background job to foreground

### Examples
```bash
mantish ls -la > files.txt
mantish grep "main" *.c | wc -l
mantish make &
mantish jobs
[1] Running    make
mantish fg 1
mantish cd .. && pwd
mantish cd -
```

## Implementation Details

### Process Management
- Fork/exec pattern for command execution
- Background process tracking in job table
- Process cleanup with waitpid()

### Memory Management
- Circular buffer for command history (configurable size)
- Dynamic string allocation for commands
- Static job array with 64 job limit

### Parsing
- Tokenization with quote and escape handling
- Operator recognition for redirection and pipes
- Command structure building for execution

### I/O Handling
- File descriptor manipulation with dup2()
- Pipe creation and management
- Standard stream redirection

## Architecture

```
src/
├── main.c          # Main shell loop
├── shell.c         # read, parse, executing
├── built_in.c      # Built-in commands
├── operators.c     # handling operators and I/O controls
├── parse.c         # extra support functions for parsing commands
├── utils.c         # extra support miscellaneous functions
├── queue.c         # implementing circular queue for command history
├── debug.c         # helper functions for printing various data-structures
```

## Testing

```bash
# Background jobs
mantish$ sleep 10 &
mantish$ jobs
mantish$ fg 1

# Redirection
mantish$ ls > output.txt
mantish$ cat < input.txt

# Pipes
mantish$ ps aux | grep mantish

# Directory operations
mantish$ cd ..
mantish$ cd -
```

## Technical Notes

- Uses POSIX system calls for process management
- Implements circular buffer for efficient history storage
- Handles file descriptor management for I/O redirection
- Custom prompt generation with git integration custom unix shell implementation in C
