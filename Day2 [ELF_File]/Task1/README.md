# The Code & The Output
we have 2 code files (main.c / file.c)
we convert them to object files by this command

>> gcc -c main.c -o main.o
>> gcc -c file.c -o file.o

then we start to link them in one excutable file 

>> gcc -static file.o main.o -o static_myProg
>> gcc file.o main.o -o noStatic_myProg

so we have 4 output files
1- main.o
2- file.o
3- static_myProg
4- noStatic_myProg

# Symbol Table

the next step we start to see the ST for the four output file by this command

>> readelf -s file.o
>> readelf -s main.o
>> readelf -s static_myProg
>> readelf -s noStatic_myProg

# Section Size

the next step we start to see the .data and .text size for the four output file by this commands

>> readelf -S file.o
>> readelf -S main.o
>> readelf -S static_myProg

>> size  file.o
>> size  main.o
>> size  static_myProg

>> objdump -h  file.o
>> objdump -h  main.o
>> objdump -h  static_myProg
