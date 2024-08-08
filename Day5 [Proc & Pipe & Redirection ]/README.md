# MyShell

MyShell is a custom shell program written in C that parses user input and translates it into commands. It supports a variety of built-in commands, external commands, input/output redirection, and piping.
 
 ## Here's a draft for my `README.md`

- Built-in Commands
- External Commands
- Extra Features
- Compilation Instructions
- Example Usage&Corner Cases
- Memory Management
- Error Handling
- File Descriptions

---
## Features

### 1 - Built-in Commands

1. **mypwd**: Print the working directory.
   - Usage: `mypwd`
   
2. **myecho**: Print a user input string on stdout.
   - Usage: `myecho Hello World`

3. **mycp**: Copy a file to another file.
    
    ex >>> `mycp <sourcePath>  <targetPath>`
   - If the target file exists, print an error.
   - If the user provides the target file name, it uses this name. 
   - If the user provides the target path without a file name, it uses the same source file name.
    -  it accepts the relative pathes and the abslute also.

   - ther is added option to append the source content to the end of the target file.
   
    ex >>> `mycp -a <sourcePath>  <targetPath>`

4. **mymv**: Move a file to another place.
    
    ex >>> `mymv <sourcePath> <targetPath>`
   - If the target file exists, print an error.
   - If the user provides the target file name,it uses this name
   -  If the user provides the target path without a file name, it uses the same source file name.
   -  it accepts the relative pathes and the abslute also.
   - ther is added option to force overwriting the target file if it exists.
   
    ex >>> `mymv -f <sourcePath>  <targetPath>` 

5. **cd**: Change the current directory.
   
    ex >>> `cd <path>`
    -  `cd` >> will Change the current directory to the HOME
    -  `cd .` --->> won't change any thing
    -  `cd ..` --->> will Change the current directory to the parent of the Current working directry

    -  `cd <first option> <second option>` --->> print error , too much argument

6. **type**: Return the type of the command (internal, external, or unsupported command).
    
    ex >>> `type command`

7. **envir**: Print all the environment variables.
   
    ex >>> `envir`

8. **free**: Print RAM info (total size, used, and free) and Swap area info (total size, used, free).
   
    ex >>> `free`

9. **uptime**: Print the uptime for the system and the time spent in the idle process.
   
    ex >>> `uptime`

10. **phist**: display the name and status and id of all created children withen this shell.
   
    ex >>> `phist`

10. **myhelp**: Print all the supported commands with brief info about each one.
    
    ex >>> `myhelp`

11. **exit**: Print "goodbye" and terminate the shell.
    
    ex >>> `exit`
---
### 2- External Commands

   - Support executing programs using their names directly without needing to enter the full path.
   - Usage: Any command available in the PATH environment variable.
---
### 3- Extra Features

1. **Input, Output, and Error Redirection**: 
   - Support input (`<`), output (`>`), and error (`2>`) redirection.
   
    ex >>> `{command}  > outputfile`

    ex >>> `{command}  < inputfile` 
    
    ex >>> `{command} 2> errorfile`
    
    - it allows the difference order for the redirection
    
    ex >>> `{command} > outputfile < inputfile 2> errorfile`

2. **Piping**:
   - Support piping between two processes.
   
    ex >>> `<command1> | <command2>`

3. **Local and Environment Variables**:
   - Support adding local and environment variables.
   
   ex >>> `addenv <Var_Name> = <Var_Value>` to set an environment variable.

   ex >>> `addloc <Var_Name> = <Var_Value>` to set a local variable.
   
   - Command `envir` will print the environment variables.
   - Command `allVar` will print both local and environment variables.

---
## Compilation Instructions
To compile the shell program, use the following command:
```sh
gcc   shell.c command.c  command.h -o shell.exe
```

## Memory Management

- Dynamic memory allocation is used for handling command arguments and process tables.
- All allocated memory is freed appropriately to prevent memory leaks.
- Special care is taken to handle reallocation and ensure no memory is wasted or leaked.

## Error Handling

- All system calls and library functions are checked for errors.
- Appropriate error messages are displayed using `perror` or custom messages.
- The shell handles common errors gracefully without crashing.

## Example Usage&Corner Cases

```sh
$ ./myShell.exe
gemy_user:/home/user << BASH MASR :) >> mypwd
/home/user
gemy_user:/home/user << BASH MASR :) >> pwd
/home/user
gemy_user:/home/user << BASH MASR :) >> 
gemy_user:/home/user << BASH MASR :) >> 
gemy_user:/home/user << BASH MASR :) >> mypwd anyargument
mypwd:error : too much argument
gemy_user:/home/user << BASH MASR :) >> myecho Hello World
Hello World
gemy_user:/home/user << BASH MASR :) >> 
gemy_user:/home/user << BASH MASR :) >>
gemy_user:/home/user << BASH MASR :) >> mycp source.txt dest.txt
gemy_user:/home/user << BASH MASR :) >> 
gemy_user:/home/user << BASH MASR :) >> mymv source.txt new_location/
gemy_user:/home/user << BASH MASR :) >> 
gemy_user:/home/user << BASH MASR :) >> cd ..
Current Working Directory : /home
gemy_user:/home/user << BASH MASR :) >> cd /home/user
Current Working Directory : /home/user
gemy_user:/home << BASH MASR :) >>cd ..
Current Working Directory : /home
gemy_user:/home << BASH MASR :) >>
gemy_user:/home << BASH MASR :) >> type mycp
mycp : internal command
gemy_user:/home << BASH MASR :) >> type ls
mycp : external command
gemy_user:/home << BASH MASR :) >> type notsupport
notsupport : unsupported command
gemy_user:/home << BASH MASR :) >> type ls anyargument
type : too much argument
gemy_user:/home << BASH MASR :) >>
gemy_user:/home << BASH MASR :) >> envir
PATH=/usr/bin:/bin
HOME=/home/user
PWD=/home
...
gemy_user:/home << BASH MASR :) >>envir PWD
PWD=/home
gemy_user:/home << BASH MASR :) >>envir nonexist
envir : nonexist doesn't exist in environment variables    
gemy_user:/home << BASH MASR :) >>envir PWD anyargument
envir : too much argument\n
gemy_user:/home << BASH MASR :) >>
gemy_user:/home << BASH MASR :) >> phist
The Children data : 
 N >> Normal Exit   T >> Terminated By Signal   S >> Stopped By Signal   U >> Undetermined
 Child 0     ID : 1234     Status : N    Process Named : ls
 Child 1     ID : 5678     Status : T    Process Named : pwd
 ....
gemy_user:/home << BASH MASR :) >> phist anyargument
phist :error, too much argument
gemy_user:/home << BASH MASR :) >>
gemy_user:/home << BASH MASR :) >> free
MEM_Total   : 8192  
MEM_Used    : 4096 
MEM_Free    : 4096
=========================
SWAP_Total  : 2048
SWAP_Used   : 1024 
SWAP_Free   : 1024
gemy_user:/home << BASH MASR :) >> 
gemy_user:/home << BASH MASR :) >> free anyargument
myfree : too much argument
gemy_user:/home << BASH MASR :) >> uptime
UP_Time      :12345.67s
UP_Time   >> Hours   :3h
          >> Minutes :25m
          >> Seconds :45.67s
==========================
IDLE_Time    :6789.10s
IDLE_Time >> Hours   :1h
          >> Minutes :53m
          >> Seconds :9.10s
gemy_user:/home << BASH MASR :) >> 
gemy_user:/home << BASH MASR :) >> ls / | grep home
home
gemy_user:/home << BASH MASR :) >> pwd > outfile
gemy_user:/home << BASH MASR :) >> cat outfile
/home
gemy_user:/home << BASH MASR :) >>
gemy_user:/home << BASH MASR :) >> mypwd < input.txt
/home
gemy_user:/home << BASH MASR :) >> ls nodirecrory 2> error.txt
gemy_user:/home << BASH MASR :) >> cat error.txt
ls : no such file or directory
gemy_user:/home << BASH MASR :) >> addloc localVAR = 10
gemy_user:/home << BASH MASR :) >> addenv EnvVAR = 20
gemy_user:/home << BASH MASR :) >> envir
PATH=/usr/bin:/bin
HOME=/home/user
PWD=/home
...
EnvVAR=20
gemy_user:/home << BASH MASR :) >> allVar
localVAR=10
===================
PATH=/usr/bin:/bin
HOME=/home/user
PWD=/home
...
EnvVAR=20
gemy_user:/home << BASH MASR :) >> myhelp
Supported Internal Commands are :
1- mycp   : copy a file to another file
>>>>>>>>> " mycp <src path> <dest path> "
>>>>>>>>> " mycp -a <src path> <dest path> " allow append option
2- myecho : print message on the screen
>>>>>>>>> " myecho <message> " 
3- mymv   : move file from directory to another directory
>>>>>>>>> " mymv <src path> <dest path> "
>>>>>>>>> " mymv -f <src path> <dest path> " allow force moving option if the file exists
4- exit   : exit from the program
5- mypwd  : print working directory
6- myhelp : show supported commands
7- cd     : change the working directory
8- envir  : print all  environment variables
9- type   : show the type of the command [Internal, External, UnSupported]
10- phist : list the process with their status 
11- free  : print RAM info (total, free, used) and swap info (total, free, used)
12- myuptime : print the uptime on the system and idle time
13- addloc: create new local variable
>>>>>>>>>> " addloc <Var_Name> = <Var_Value>> "
14- addenv: create new environment variable
>>>>>>>>>> " addenv <Var_Name> = <Var_Value>> "
15- allVar: print environment variable and local
16- output redirection " {command} > {output_file} "
17- input redirection  " {command} < {input_file} "
18- error redirection  " {command} 2> {error_file} "
19- pipe way  " {command_1} | {command_2} "

gemy_user:/home << BASH MASR :) >> exit
Good Bye :)

gemy_user:/home $
```
## File Descriptions

### `shell.c`

Contains the main function and the shell loop. It handles command input, parsing, and execution. It includes the main logic for identifying built-in versus external commands.

### `command.c`

Implements the functions for built-in commands and other utilities. This includes functions like `mycp`, `mymv`, `mypwd`, `myecho`, `myhelp`, `myexit`, `cd`, `type`, and `envir`.

### `command.h`

Header file containing declarations for the functions implemented in `command.c`.
