# MyShell (BASHA MASR)

 ## Here's a draft for my `README.md`

- Project Overview
- Compilation Instructions
- Built-in Commands
- External Commands
- Extra Features
- Memory Management
- Error Handling
- Test Cases and Examples
- File Descriptions


## Project Overview

The MyShell (BASHA MASR) project is a custom-built command-line interpreter designed to mimic and enhance the features of a typical Unix-like shell. 

MyShell offers an interactive interface where users can run commands, manage files, and perform various system-level operations.

Alongside supporting standard Unix commands, MyShell includes a collection of built-in commands, providing a more customized command-line experience to meet specific requirements.

It effectively supports a range of built-in commands, external commands, and advanced functionalities like input/output redirection , piping and environment variable handling.

Developed in C, this project serves as a learning tool to explore the complexities of shell programming, process control, and system calls within a Unix-like operating system.


## Compilation Instructions
- To compile the shell program, use the following command:
```sh
gcc  shell.c command.c  command.h -o shell
```
- To Run the Shell
```sh
./shell
```
- User Interface(You will see in the screen `)
```sh
<USER_NAEME> : <CURRENT_WORKING_DIRECTORY> << BASHA MASR :) >>             `
```

---
## Features 

### 1 - Built-in Commands

1. **mypwd** : Print the working directory.
  
    #### ex >>> `mypwd`
   - `mypwd <anyargument>` --->> print error , too much argument
 
2. **myecho** : Print a user input string on stdout.
   
    #### ex >>> `myecho <printed message>`

3. **mycp** : Copy a file from src path to dest path .
    
    #### ex >>> `mycp <sourcePath>  <targetPath>`
   -  `mycp  /path1/file  /path2/existfile` --->> If the target file exists, print an error.
   - `mycp  /path1/notexistfile  /path2/file` --->> If the source file doesn't exist ,print error
   - `mycp  /path1/file1  /path2/file2` --->> If the user provides the target file name, it uses this name (at this case we copy the content of the source file(file1) to the target file(file2)) 
   - `mycp  /path1/file1  /path2/` --->> If the user provides the target path without a file name, it uses the same source file name.
    -  `mycp  file1  ./directory` --->> it accepts the relative pathes and the abslute also.

   - ther is added option (-a) to append the source content to the end of the target file .
   
    #### ex >>> `mycp -a <sourcePath>  <targetPath>`
    - `mycp  file1  file2 anyargument ` --->> print error , too much argument
    
4. **mymv** : Move a file from place to another place.
    
    #### ex >>> `mymv <sourcePath> <targetPath>`

    -  `mymv  /path1/file  /path2/existfile` --->> If the target file exists, print an error.
   - `mymv  /path1/notexistfile  /path2/file` --->> If the source file doesn't exist ,print error
   - `mymv  /path1/file1  /path2/file2` --->> If the user provides the target file name, it uses this name (at this case we move the source file(file1) from path1 to path 2 and change his name to the target file name(file2)) 
   - `mymv  /path1/file1  /path2/` --->> If the user provides the target path without a file name, it uses the same source file name (at this case we move the source file(file1) from path1 to path 2 with the same named of the source file (file1))
   
    - `mymv  file1  ./directory` --->> it accepts the relative pathes and the abslute also

   - ther is added option (-f) to force overwriting if the target file already exists.
   

    #### ex >>> `mymv -f   /path1/file  /path2/existfile`
   
    - `mymv  file1  file2  anyargument ` --->> print error , too much argument
 
5. **cd** : Change the current directory.
   
    #### ex >>> `cd <path>`
    -  `cd` --->> will Change the current directory to the HOME
    -  `cd .` --->> won't change any thing
    -  `cd ..` --->> will Change the current directory to the parent of the Current working directry
    -   it accepts the relative pathes and the abslute also
    -  `cd /home/path` --->> abslute path
    -  `cd directory/subdirectory`--->> relative path
    -  `cd <path> <anyargument>` --->> print error , too much argument

6. **type** : Return the type of the command (internal, external, or unsupported command).
    
    #### ex >>> `type <command>`
    -  `type` --->> print error , where is the command
    -  `type <command> <anyargument>` --->> print error , too much argument
    
7. **envir** : Print all the environment variables.
   
    #### ex >>> `envir`
    - it has option to print certain environment variable
    - `envir <environment variable name>` --->> display this enviornment variable if exists
    - `envir PATH` --->> will print the content of this variable "PATH"
    -  `envir nonexistvar` -->> will print error , not found
    -  `envir <variable> <anyargument>` --->> print error , too much argument
    

8. **myfree** : Print RAM info (total size, used, and free) and Swap area info (total size, used, free).
   
    #### ex >>> `myfree`
    -  `myfree <anyargument>` --->> print error , too much argument
    
9. **myuptime** : Print the uptime for the system and the time spent in the idle process.
   
    #### ex >>> `myuptime`
    -  `myuptime <anyargument>` --->> print error , too much argument

10. **phist** : display the name, status,PID and id of all created children withen this shell.
   
    #### ex >>> `phist`
     -  `phist <anyargument>` --->> print error , too much argument


10. **myhelp** : Print all the supported commands with brief info about each one.
    
    #### ex >>> `myhelp`
    -  this command has perfect guide to use the internal commands
    -  `myhelp <anyargument>` --->> print error , too much argument
11. **exit** : Print "goodbye" and terminate the shell.
    
    #### ex >>> `exit`
     -  `exit <anyargument>` --->> print error , too much argument

13. **addenv** : create new environment varible  .
    
    #### ex >>> `addenv Env_Var_Name = Env_Var_Value`
    - if the Env_Var_Name exist , it will overwrite on it
     -  if you don't follow the above way --->> print error , write "myhelp" command to see the correct way
 
14. **addloc** : create new local variable  .
    
    #### ex >>> `addloc Loc_Var_Name = Loc_Var_Value`
    - if the Loc_Var_Name exist , it will overwrite on it
     -  if you don't follow the above way --->> print error , write "myhelp" command to see the correct way

15. **allVar** : print all environment varibles and local variables   .
    
    #### ex >>> `allVar`
     -  `allVar <anyargument>` --->> print error , too much argument
---
### 2- External Commands
- Support executing programs using their pathes (relative or abslute)

 - Support executing programs using their names directly without needing to enter the full path.
    -   Any command available in the PATH environment variable.
---
### 3- Extra Features

1. **Input, Output, and Error Redirection**: 
  - Support input (`<`), output (`>`), and error (`2>`) redirection.
   
    #### ex >>> `{command}  > outputfile`

    #### ex >>> `{command}  < inputfile`   
    
    #### ex >>> `{command} 2> errorfile`
    
- it allows the difference order for the redirection
    
    #### ex >>> `{command} > outputfile < inputfile 2> errorfile`

2. **Piping**:
  - Support piping between any numbers of commands(Multi piping).
   
    #### ex >>> `<command1> | <command2> | <command3> | ... | ...`
- it avoids missing of any commands --->> print error and you can write "myhelp" to see help
    - `<command1> |  | <command3>`
    - `<command1> |  `
    - `| <command3>`
- Don't forget the separating spaces 

3. **Local and Environment Variables**:
   - Support adding local and environment variables.
   
   ex >>> `addenv <Var_Name> = <Var_Value>` to set an environment variable.

   ex >>> `addloc <Var_Name> = <Var_Value>` to set a local variable.
   
   - Command `envir` will print the environment variables.
   - Command `allVar` will print both local and environment variables.

---

## Memory Management

- Dynamic memory allocation is used for handling command arguments and process tables.
- All allocated memory is freed appropriately to prevent memory leaks.
- Special care is taken to handle reallocation and ensure no memory is wasted or leaked.

## Error Handling

- All system calls and library functions are checked for errors.
- Appropriate error messages are displayed using `perror` or custom messages.
- The shell handles common errors gracefully without crashing.

## Test Cases and Examples

1. **Change Directory**

    ```sh
    BASHA MASR :) >> cd /path/to/directory
    Current Wording Directory : /path/to/directory
    BASHA MASR :) >> cd /path/to/nonexistent_directory
    cd : no such file or directory
    BASHA MASR :) >> cd /file
    cd : not a directory
    BASHA MASR :) >> cd /home/ST
    Current Wording Directory : /home/ST
    BASHA MASR :) >> cd ..
    Current Wording Directory : /home
    BASHA MASR :) >> cd /home/ST
    Current Wording Directory : /home/ST
    BASHA MASR :) >> cd /home/ST anyargument
    cd : too much argument
    ```

5. **Free info**

     ```sh
    BASHA MASR :) >> myfree
    MEM_Total   : 8192  
    MEM_Used    : 4096 
    MEM_Free    : 4096
    =========================
    SWAP_Total  : 2048
    SWAP_Used   : 1024 
    SWAP_Free   : 1024
    BASHA MASR :) >> 
    BASHA MASR :) >> myfree anyargument
    myfree : too much argument
     ```
6. **Echo Command**
    
    ```sh
    BASHA MASR :) >> myecho Hello, World!
    Hello, World!
    BASHA MASR :) >> myecho Hello, World! extraArgument
    myecho: too much argument
    ```
7. **Print Working Directory**

    ```sh
    BASHA MASR :) >> mypwd
    /home/ST
    BASHA MASR :) >> mypwd extraArgument
    mypwd: too much argument
 
    ```
8. **Check The Type of Command**

    ```sh
    BASHA MASR :) >> type mypwd
    mypwd : Internal Command
    BASHA MASR :) >> type ls
    ls : External Command
    BASHA MASR :) >> type nonexistcommand
    nonexistcommand : unsupported command
    BASHA MASR :) >> type command extraArgument
    type : too much argument
    ```
10. **Help with Internal Command**

    ```sh
    BASHA MASR :) >> myhelp
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
    11- myfree  : print RAM info (total, free, used) and swap info (total, free, used)
    12- myuptime : print the uptime on the system and idle time
    13- addloc: create new local variable
    >>>>>>>>>> " addloc <Var_Name> = <Var_Value>> "
    14- addenv: create new environment variable
    >>>>>>>>>> " addenv <Var_Name> = <Var_Value>> "
    15- allVar: print environment variable and local
    16- output redirection " {command} > {output_file} "
    17- input redirection  " {command} < {input_file} "
    18- error redirection  " {command} 2> {error_file} "
    NOTE : you can do many redirection at the same command with different order [don't forget the spaces] 
    ex>>>> {command} 2> {error_file} > {output_file} < {input_file} 
    19- pipe way  " {command_1} | {command_2} | {command_3} | ... | ... 
    NOTE : You can do multi pipes but don't forget the separating spaces
    BASHA MASR :) >> myhelp extraArgument
    myhelp : too much argument
    ```
10. **Display Environment Variable**
    
    ```sh
    BASHA MASR :) >>
    BASH MASR :) >> envir
    PATH=/usr/bin:/bin
    HOME=/home/user
    PWD=/home
    ...
    BASH MASR :) >>envir PWD
    PWD=/home
    BASH MASR :) >>envir nonexist
    envir : nonexist doesn't exist in environment variables    
    BASH MASR :) >>envir PWD anyargument
    envir : too much argument
    ```

11. **Display Info of the Children**
    
    ```sh
    BASH MASR :) >>
    BASH MASR :) >> phist
    The Children data : 
     N >> Normal Exit   T >> Terminated By Signal   S >> Stopped By Signal   U >> Undetermined
     Child 0     ID : 1234      PID : 1111     Status : N    Process Named : ls
     Child 1     ID : 5678      PID : 1111     Status : T    Process Named : pwd
     ....
    BASH MASR :) >> phist anyargument
    phist :error, too much argument
    ```
12. **Add local and environment Varible and Display it**
 
    ```sh
    BASHA MASR :) >> addloc localVAR = 10
    BASHA MASR :) >> addenv EnvVAR = 20
    BASHA MASR :) >> envir
    PATH=/usr/bin:/bin
    HOME=/home/user
    PWD=/home
    ...
    EnvVAR=20
    BASHA MASR :) >> allVar
    localVAR=10
    ===================
    PATH=/usr/bin:/bin
    HOME=/home/user
    PWD=/home
    ...
    EnvVAR=20
    BASHA MASR :) >> addloc localVAR = 100
    BASHA MASR :) >> addenv EnvVAR = 200
    BASHA MASR :) >> allVar
    localVAR=100
    ===================
    PATH=/usr/bin:/bin
    HOME=/home/user
    PWD=/home
    ...
    EnvVAR=200
    BASHA MASR :) >> addloc
    addloc : incorrect way to assign local variable
    correct way >> "addloc <VarName> = <VarValue>"
    BASHA MASR :) >> addenv
    addenv : incorrect way to assign environment variable
    correct way >> "addenv <VarName> = <VarValue>"
    ```
13. **Uptime Info**

    ```sh
    BASH MASR :) >> myuptime
    UP_Time      :12345.67s
    UP_Time   >> Hours   :3h
              >> Minutes :25m
              >> Seconds :45.67s
    =============================
    IDLE_Time    :6789.10s
    IDLE_Time >> Hours   :1h
              >> Minutes :53m
              >> Seconds :9.10s
    BASH MASR :) >> myuptime extraArgument
    myuptime : too much argument
    ```
    
14. **Exit From Program**

    ```sh
    BASHA MASR :) >> exit extraArgument
    exit : too much argument
    BASHA MASR :) >> exit
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
