# `ls` Utility Implementation

## 1. Features

This implementation of the `ls` command replicates the functionality of the standard UNIX `ls` utility with advanced options and color-coded output based on file types.

### Supported Options:
- **`-l`**: Long listing format showing file metadata (permissions, owner, group, size, etc.).
- **`-a`**: Include hidden files (starting with a dot).
- **`-t`**: Sort files by modification time.
- **`-u`**: Sort by access time.
- **`-c`**: Sort by inode change time.
- **`-i`**: Display inode numbers.
- **`-f`**: Disable sorting and list files as they appear.
- **`-d`**: List directories themselves, not their contents.
- **`-1`**: List one file per line.
- **`--color`**: Display file names in different colors based on file type (e.g., directories in blue, executables in green).

### File Types and Corresponding Colors:
- **Directories**: Blue
- **Regular Files**: Default
- **Executable Files**: Green
- **Symbolic Links**: Cyan
- **Block Devices**: Yellow
- **Character Devices**: Yellow
- **FIFO**: Purple
- **Sockets**: Green

### Additional Features:
- Supports multiple directories.
- Output is formatted in a neatly aligned table.

## 2. Code Flow

### Step 1: **Parsing Command-Line Arguments**
- Command-line arguments are processed using `getopt()` to support options like `-l`, `-a`, `-t`, and `--color`.

### Step 2: **Reading Directory Contents**
- The contents of directories are retrieved using system calls like `opendir()`, `readdir()`, and `lstat()`, and the metadata of each file is captured.

### Step 3: **Sorting**
- Sorting is done based on modification time (`-t`), access time (`-u`), or inode change time (`-c`) using `qsort()`.

### Step 4: **Output Formatting**
- The output is formatted in a long listing (`-l`) or one file per line (`-1`). Additional metadata like inode numbers (`-i`) or directories (`-d`) is printed based on the selected options.

### Step 5: **Color Support**
- When `--color` is specified, files are printed in colors based on their type (e.g., directories in blue, executables in green).

## 3. Example Usages and Expected Output

### 1. **Basic Listing**:
```bash
./myls
```
Expected output:
```
file1  file2  file3
```

### 2. **List all files, including hidden ones (`-a`)**:
```bash
./myls -a
```
Expected output:
```
.  ..  .hidden_file  file1  file2  file3
```

### 3. **Long listing format (`-l`)**:
```bash
./myls -l
```
Expected output:
```
-rw-rw-r-- 1 user user  1040 Sep 19 18:57 file1
-rwxrwxr-x 1 user user  2050 Sep 19 18:55 file2
drwxrwxr-x 2 user user  4096 Sep 19 21:02 dir1
```

### 4. **Show inode numbers (`-i`)**:
```bash
./myls -i
```
Expected output:
```
131047 file1
131048 file2
131049 dir1
```

### 5. **Combine options: Long listing, sorted by modification time, showing hidden files (`-lat`)**:
```bash
./myls -lat
```
Expected output:
```
-rwxrwxr-x 1 user user  2050 Sep 19 18:55 file2
-rw-rw-r-- 1 user user  1040 Sep 19 18:57 file1
drwxrwxr-x 2 user user  4096 Sep 19 21:02 dir1
```

### 6. **Display with colors (`--color`)**:
```bash
./myls --color
```
Expected output (file names in colors):
```
dir1  file1  file2
```
- `dir1`: Blue (directory)
- `file2`: Green (executable)
- `file1`: Default color (regular file)

### 7. **List directories only (`-d`)**:
```bash
./myls -d /path/to/directory
```
Expected output:
```
/path/to/directory
```

### 8. **List one file per line (`-1`)**:
```bash
./myls -1
```
Expected output:
```
file1
file2
file3
dir1
```

### 9. **Disable sorting (`-f`)**:
```bash
./myls -f
```
Expected output (no sorting):
```
file2  dir1  file1
```

## 4. Screenshots of Output

### Example 1: Basic listing with color:
![Basic listing with color](https://drive.google.com/file/d/107U0AUGoA2QLUpCbA_Vqr7Oh9MnoYEba/view?usp=sharing)

### Example 2: Long listing format (`-l`) with inode numbers (`-i`) with color (`--color`) with hidden files (`-a`):
![Long listing with inode numbers](https://drive.google.com/file/d/1L3OA79Plb-xN11WTDGwgrzTC-KM0ATWl/view?usp=sharing)

## 5. Conclusion

This custom implementation of the `ls` command supports a variety of features like long listing, inode numbers, sorting by time, and color-coding. It closely mimics the behavior of the standard `ls` utility in UNIX-like systems, and additional features can be easily extended. The code is modular, and the options are flexible, allowing for easy testing and future enhancements.

--- 
