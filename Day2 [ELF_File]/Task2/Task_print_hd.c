#include <stdio.h>

#include <elf.h>

//Absolute PathFILE
#define  FILE_PATH	("/home/vboxuser/Desktop/ST_WorkSpace/DAY2/file.exe")
#define READ_ONLY_MODE	("r")

Elf64_Ehdr headerStr;
// arch[0] >>> name of the file 
// arch[1] >>> 
int main(int argc,char **argv)
{	
	FILE *elfFile = NULL;
	elfFile = fopen(argv[1],READ_ONLY_MODE); 
	fread(&headerStr, sizeof(headerStr), 1, elfFile);
	fclose(elfFile);
	printf("ELF Header: \n");
        printf("  Magic:    ");
     	for(int i = 0;i<EI_NIDENT;i++)
	{
		printf("%x ",headerStr.e_ident[i]);
	}	
	printf("\n");

    printf("  Class                              :%d\n", headerStr.e_ident[EI_CLASS]);
    printf("  Data                               :%d\n", headerStr.e_ident[EI_DATA]);
    printf("  Version                            :%d\n", headerStr.e_ident[EI_VERSION]);
    printf("  OS/ABI                             :%d\n", headerStr.e_ident[EI_OSABI]);
    printf("  ABI Version                        :%d\n", headerStr.e_ident[EI_ABIVERSION]);

	char *arr_Type[] = {
		"NONE (Unknown type)",
                "REL (Relocatable file)",
                "EXEC (Executable file)",
                "DYN (Shared object)",
                "CORE (Core file)"};
	
	printf("  TYPE                               : %s\n",arr_Type[headerStr.e_type]);

	char *arr_Machine[] = {
                "NONE (unknown machine)",
		"M32 (AT&T WE 32100)",
		"SPARC (Sun Microsystems SPARC)",
		"386 (Intel 80386)",
		"68K (Motorola 68000)",
		"88K (Motorola 88000)",
		"860 (Intel 80860)",
		"MIPS (MIPS RS3000 big-endian only)",
		"PARISC (HP/PA)",
		"SPARC32PLUS (SPARC with enhanced instruction set)",
		"PPC (PowerPC)",
		"PPC64 (PowerPC 64-bit)",
		"S390 (IBM S/390)",
		"ARM (Advanced RISC Machines)",
		"SH (Renesas SuperH)",
		"SPARCV9 (SPARC v9 64-bit)",
		"IA_64 (Intel Itanium)",
		"X86_64 (AMD x86-64)",
		"VAX (DEC Vax)"};
	int i =0;
    switch (headerStr.e_machine) {
        case EM_NONE:
            i = 0;
            break;
        case EM_M32:
            i = 1;
            break;
        case EM_SPARC:
            i = 2;
            break;
        case EM_386:
            i = 3;
            break;
        case EM_68K:
            i = 4;
            break;
        case EM_88K:
            i = 5;
            break;
        case EM_860:
            i = 6;
            break;
        case EM_MIPS:
            i = 7;
            break;
        case EM_PARISC:
            i = 8;
            break;
        case EM_SPARC32PLUS:
            i = 9;
            break;
        case EM_PPC:
            i = 10;
            break;
        case EM_PPC64:
            i = 11;
            break;
        case EM_S390:
            i = 12;
            break;
        case EM_ARM:
            i = 13;
            break;
        case EM_SH:
            i = 14;
            break;
        case EM_SPARCV9:
            i = 15;
            break;
        case EM_IA_64:
            i = 16;
            break;
        case EM_X86_64:
            i = 17;
            break;
        case EM_VAX:
            i = 18;
            break;
        default:
            printf("Unknown machine type\n");
            break;
    }
        printf("  Machine                            : %s\n",arr_Machine[i]);



	char *arr_Version[] = {
                "NONE (Invalid version)",
                "CURRENT (Current version)"};

        printf("  Version                            : %s\n",arr_Version[headerStr.e_version]);

	printf("  Entry point address                : 0x%x\n",headerStr.e_entry);
        printf("  Start of program headers           : %d (bytes into file)\n",headerStr.e_phoff);
        printf("  Start of section headers           : %d (bytes into file)\n",headerStr.e_shoff);
        printf("  Flags                              : 0x%x\n",headerStr.e_flags);
	printf("  Size of this header                : %d (bytes)\n",headerStr.e_ehsize);
        printf("  Size of program headers            : %d (bytes)\n",headerStr.e_phentsize);
        printf("  Number of program headers          : %d \n",headerStr.e_phnum);
        printf("  Size of section headers            : %d (bytes)\n",headerStr.e_shentsize);
        printf("  Number of section headers          : %d \n",headerStr.e_shnum);
        printf("  Section header string table index  : %d \n",headerStr.e_shstrndx);








}

