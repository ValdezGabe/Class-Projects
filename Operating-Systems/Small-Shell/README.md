# Small Shell — Operating Systems Project

## Overview
This project is a custom command-line shell written in C that mimics basic functionality of popular Unix shells. It supports foreground and background execution, built-in commands, input/output redirection, signal handling, and variable expansion. The shell was designed for an Operating Systems course to demonstrate process control, signal management, and I/O redirection in a Linux environment.

## Details
- `smallsh.c`: The main shell implementation.
- Supports:
  - Built-in commands: `exit`, `cd`, and `status`
  - Background execution with `&`
  - Input (`<`) and output (`>`) redirection
  - Signal handling for `SIGINT` and `SIGTSTP`
  - Variable expansion: `$$` expands to the shell’s process ID

## A. Compiling the Program

### Instructions
1. Open a terminal and navigate to the `Small-Shell` directory.
2. Compile the shell using:
   ```bash
   gcc -o smallsh smallsh.c
   ```
## B. Running the Shell

### Instructions
1. Run the compiled shell:
  ```bash
  ./smallsh
  ```
2. The shell will display a prompt (:) where you can begin typing commands.
