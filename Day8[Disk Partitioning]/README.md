
# My Fdisk Implementation

## Overview

This project implements a basic version of the `fdisk -l` command in C, which reads the partition table from a disk using both the MBR (Master Boot Record) and GPT (GUID Partition Table) formats. The program identifies and displays partition information, including logical partitions in MBR and various partition types in GPT.

## Features

- **MBR Support**: The program reads and displays both primary and extended partitions.
- **Logical Partitions**: Logical partitions within an extended partition in MBR are displayed if they exist.
- **GPT Support**: The program parses the GPT and shows detailed information, including the type of partition based on GUIDs.
- **Formatted Output**: The output is formatted in a table-like structure for easy reading, with aligned columns.
- **Error Handling**: Checks return values from system calls and library routines to handle errors gracefully.
## Compilation

To compile the program, use the following command:

```bash
gcc -o myfdisk myfdisk.c
```

Make sure your system has a C compiler installed.

## Usage

Run the program by passing the disk device as an argument. For example:

```bash
./myfdisk /dev/sda
```

This will output the partition information for the specified disk.

## Example Output

The following is an example of the output format for both MBR and GPT partitions:

### MBR Example:
```
Disklabel type: MBR
Device     Boot       Start      End        Sectors    Size       Id         Type    
/dev/sdb1             2048       2099199    2097152    1G 	  83         Linux 
/dev/sdb2             2099200    4194303    2095104    1023M 	  5          Extended 
/dev/sdb5             2101248    2510847    409600     200M 	  83         Linux 
/dev/sdb6             2512896    2717695    204800     100M 	  83         Linux 
/dev/sdb7             2719744    3129343    409600     200M 	  83         Linux
```

### GPT Example:
```
Disklabel type: GPT
Device     Start      End        Sectors    Size          Type 
/dev/sda1  2048       4095       2048       1024K 	  BIOS_BOOT 
/dev/sda2  4096       1054719    1050624    513M 	  EFI_SYSTEM 
/dev/sda3  1054720    52426751   51372032   25084M 	  LINUX_FILESYST

```

## Implementation Details

Here’s a brief description of how the program is structured:

- **MBR Parsing**: The program reads the first 512 bytes of the disk, identifies the MBR, and extracts primary and extended partition entries.
- **GPT Parsing**: The program reads the GPT header and partition entries, then maps the GUIDs to known partition types.
- **Formatted Printing**: The output is aligned using `printf` formatting, ensuring that each column (e.g., Start Sector, End Sector, Size, Type) is neatly aligned for readability.

## Error Handling

The program includes basic error handling:
- Checks if the specified disk device can be opened.
- Validates the presence of MBR or GPT signatures.
- Handles cases where no partitions are found or the table format is unsupported.


