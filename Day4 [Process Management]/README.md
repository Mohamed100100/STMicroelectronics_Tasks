# Shell - Version 2

## Overview

This project implements a custom shell with both built-in and external command support. The shell extends its capabilities to handle additional commands and features dynamic memory allocation for flexibility.

## Features

### Built-in Commands
1. **Old Commands**
   - `mycp`: Copy files
   - `mymv`: Move files
   - `mypwd`: Print working directory
   - `myecho`: Echo text
   - `myhelp`: Display help message
   - `myexit`: Exit the shell

2. **New Commands**
   - `cd path`: Change the current directory
   - `type command`: Return the type of the command (internal, external, or unsupported)
   - `envir`: Print all the environment variables (same as `env`)
   - **Bonus**:
     - `phist`: List the last 10 processes with their exit status
     - `envir variable`: Print the value of a specific environment variable if it exists

### External Commands
- Support executing programs using their names directly without the need to enter the full path.
- Utilizes dynamic allocation to handle a flexible number of arguments and ensures no memory leaks.
- Properly manages resources, especially child processes.


## Compilation

To compile the shell program, use the following command:

```sh
gcc -o shell shell.c command.c -I.
```

## Usage

### Running the Shell

To start the shell, run the following command after compilation:

```sh
./shell
```

### Built-in Command Examples

- Change directory:
  ```sh
  cd /path/to/directory
  ```

- Check command type:
  ```sh
  type mycp
  ```

- Print environment variables:
  ```sh
  envir
  ```

- Print a specific environment variable:
  ```sh
  envir HOME
  ```

- List the last 10 processes with their exit status:
  ```sh
  phist
  ```

### External Command Example

- Run an external command like `ls`:
  ```sh
  ls -l
  ```

## Test Cases

### Built-in Command Tests

1. **Change Directory and Print Working Directory**:
   ```sh
   cd /tmp
   mypwd
   ```
   Expected Output: `/tmp`

2. **Echo Text**:
   ```sh
   myecho Hello, World!
   ```
   Expected Output: `Hello, World!`

3. **List Environment Variables**:
   ```sh
   envir
   ```
   Expected Output: (List of all environment variables)

4. **Print Specific Environment Variable**:
   ```sh
   envir PATH
   ```
   Expected Output: (Value of PATH environment variable)

5. **Command Type Check**:
   ```sh
   type mycp
   ```
   Expected Output: `internal`

   ```sh
   type ls
   ```
   Expected Output: `external`

   ```sh
   type unknowncmd
   ```
   Expected Output: `unsupported`

6. **Process History**:
   ```sh
   phist
   ```
   Expected Output: (Last 10 processes with their exit status)

### External Command Tests

1. **List Directory Contents**:
   ```sh
   ls -l
   ```
   Expected Output: (Detailed list of directory contents)

2. **Display Current Date and Time**:
   ```sh
   date
   ```
   Expected Output: (Current date and time)

## File Descriptions

### `shell.c`

Contains the main function and the shell loop. It handles command input, parsing, and execution. It includes the main logic for identifying built-in versus external commands.

### `command.c`

Implements the functions for built-in commands and other utilities. This includes functions like `mycp`, `mymv`, `mypwd`, `myecho`, `myhelp`, `myexit`, `cd`, `type`, and `envir`.

### `command.h`

Header file containing declarations for the functions implemented in `command.c`.

