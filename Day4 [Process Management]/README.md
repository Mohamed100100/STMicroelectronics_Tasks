# Shell V2

- Project overview
- Compilation instructions
- Usage instructions
- Detailed description of each internal command
- Supported corner cases
- Test cases and examples
- Notes on memory management and system call handling
- File Descriptions

 Here's a draft for my `README.md`

---



## Project Overview

This project is an extension of a basic shell program. The shell supports both built-in and external commands, including new internal commands and features to manage processes. The shell is designed to be flexible with dynamic memory allocation and robust error handling.

## Features

- **Built-in Commands**: 
  - Old commands (renamed to avoid conflicts with external commands): `mycp`, `mymv`, `mypwd`, `myecho`, `myhelp`, `myexit`
  - New commands:
    - `cd path`: Change the current directory
    - `type command`: Return the type of the command (internal, external, or unsupported)
    - `envir`: Print all environment variables or the value of a specific environment variable
  - Bonus commands:
    - `phist`: List the last 10 processes with their exit status

- **External Commands**: 
  - Execute programs using their names directly from the PATH environment variable
  - Dynamic allocation for arguments to ensure flexibility and prevent memory leaks

## Compilation Instructions

To compile the shell program, use the following command:

```sh
gcc -o shell shell.c command.c  shell.exe
```

## Usage Instructions

Run the shell program using:

```sh
./shell.exe
```

You will see the prompt `BASHA MASR :) >>`, where you can enter your commands.

## Supported Internal Commands

### `mycp source destination`
Copy files from source to destination.
- Print error if the target file exists.
- if the user provide the target file name then use it. 
- if the user provide the target path without file name, use the  same source file name.

### `mycp -a source destination`
append the source content to the end of the target file

### `mymv source destination`
Move files from source to destination.
- Print error if the target file exists
- if the user provide the target file name then use it.
- if the user provide the target path without file name,use the same source file name.
 
### `mymv -f source destination`

Move files and force overwriting the target file if exists  

### `mypwd`
Print the current working directory.

- It is prohibited to enter more than one option
-    `mypwd <any option>` --->>> will print error
### `myecho [args]`
Echo the provided arguments.

### `myhelp`
Display help information about the shell(Internal Commands).
- #### `myhelp <option>` --->> print error , too much argument
### `exit`
Exit the shell.
- #### `exit <option>` --->> print error , too much argument


### `cd path`
Change the current directory to the specified path.
- it allow many options :-
-  `cd` --->> will Change the current directory to the HOME
-  `cd .` --->> won't change any thing
-  `cd ..` --->> will Change the current directory to the parent of the Current working directry

-  `cd {first option} {second option}` --->> print error , too much argument

### `type command`
Return the type of the specified command (internal, external, or unsupported)

- #### `type <input command> <second option>` print error , too much argument

### `envir`
Print all environment variables. If an environment variable name is provided, print its value.
- `envir {variable name}` --->> display this enviornment variable if exists

ex >> - `envir PATH` -->> will print the content of this variable
ex >> - `envir PAT` -->> will print error , not found

### `phist`
List the last 10 processes with their exit status.
- #### `phist <option>` print error , too much argument

## Supported Corner Cases

- **Empty input**: Pressing Enter without typing anything.
- **Invalid commands**: Commands that are neither internal nor found in the PATH.
- **Invalid arguments**: Incorrect number or type of arguments for commands.
- **Memory allocation failures**: Handling cases where `malloc` or `realloc` fails.
- **Fork failures**: Handling cases where `fork` fails.

## Test Cases and Examples

1. **Change Directory**

    ```sh
    BASHA MASR :) >> cd /path/to/directory
    Current Wording Directory : /path/to/directory
    BASHA MASR :) >> cd /path/to/nonexistent_directory
    cd : Invalid Path
    BASHA MASR :) >> cd .
    Current Wording Directory : /home/usr
    BASHA MASR :) >> cd ..
    Current Wording Directory : /home
    ```

2. **Type Command**

    ```sh
    BASHA MASR :) >> type mycp
    mycp : internal command
    BASHA MASR :) >> type ls
    ls : external command
    BASHA MASR :) >> type nonexistent
    nonexistent : unsupported command
    ```

3. **Environment Variables**

    ```sh
    BASHA MASR :) >> envir
    DISPLAY=:0
    HOME=/home/user
    ...
    BASHA MASR :) >> envir HOME
    HOME=/home/user
    ```

4. **Process History**

    ```sh
    BASHA MASR :) >> phist
    N >> Normal Exit   T >> Terminated By Signal   S >> Stopped By Signal   U >> Undetermined
    Child 0   ID : 1234   Status : N
    Child 1   ID : 1235   Status : T
    ...
    ```

## Memory Management

- Dynamic memory allocation is used for handling command arguments and process tables.
- All allocated memory is freed appropriately to prevent memory leaks.
- Special care is taken to handle reallocation and ensure no memory is wasted or leaked.

## Error Handling

- All system calls and library functions are checked for errors.
- Appropriate error messages are displayed using `perror` or custom messages.
- The shell handles common errors gracefully without crashing.

---

## File Descriptions

### `shell.c`

Contains the main function and the shell loop. It handles command input, parsing, and execution. It includes the main logic for identifying built-in versus external commands.

### `command.c`

Implements the functions for built-in commands and other utilities. This includes functions like `mycp`, `mymv`, `mypwd`, `myecho`, `myhelp`, `myexit`, `cd`, `type`, and `envir`.

### `command.h`

Header file containing declarations for the functions implemented in `command.c`.
