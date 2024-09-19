
# `ls` Utility Implementation

## Overview

This is a custom implementation of the `ls` command, which is widely used to list directory contents in UNIX-like operating systems. This version of `ls` supports several options including `-latucifd1`, and can organize output in a table format. A bonus feature of `--color` support is also included.

## Features

- **Options Supported**:
  - `-l`: Use a long listing format.
  - `-a`: Include entries starting with a dot (`.`).
  - `-t`: Sort by modification time, newest first.
  - `-u`: Sort by last access time.
  - `-c`: Sort by inode change time.
  - `-i`: Display the index number (inode) of each file.
  - `-f`: Disable sorting and print all files.
  - `-d`: List directories themselves, not their contents.
  - `-1`: List one file per line.
  

## Installation

To compile the `ls` implementation:

```bash
gcc -o myls myls.c  myls.h
```

## Usage

```bash
./myls [options] [directories/files]
```

### Example Usages:

1. **Basic listing**:
   ```bash
   ./myls
   ```
   Lists the contents of the current directory.

2. **List all files, including hidden ones (`.` files)**:
   ```bash
   ./myls -a
   ```

3. **Long listing format with inode numbers**:
   ```bash
   ./myls -li
   ```

4. **Sort by modification time**:
   ```bash
   ./myls -lt
   ```

5. **List directory itself, not its contents**:
   ```bash
   ./myls -d /path/to/directory
   ```

6. **List one file per line**:
   ```bash
   ./myls -1
   
   ```

## Testing

To verify the correctness and performance of the `myls` program, try the following test cases:

1. List files in a directory with various combinations of options (e.g., `-lat`, `-luc`, `-i1`).
3. Run the command in directories with nested folders.
4. Use `-d` to ensure directories are listed, not their contents.
5. Run the program in a directory with many files to check for correct table formatting.

## Notes

- The system calls used (such as `opendir()`, `readdir()`, `lstat()`, etc.) return valid results.
- The output format follows the standard Linux `ls` utility, with properly aligned columns.
  
## Video Demonstration

A short video showing the `myls` implementation and its features has been recorded. You can view the video [here](https://drive.google.com/file/d/1sg4QZYYrxAMtIxOHAdni9EYWVFR-T76E/view?usp=sharing).


