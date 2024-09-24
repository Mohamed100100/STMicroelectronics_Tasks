#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/ioctl.h>
#include <stdint.h>
#include <fcntl.h>
#include <pwd.h>
#include <time.h>
#include <grp.h>
#include <getopt.h>



#include "myls.h"


/* This Flages for options */
uint8_t Argument_Option     = OFF;    /* This Option detect if there is option or not */
uint8_t Enable_Color        = OFF;    /* This Option Enable the color printing */
uint8_t Enable_Long_Format  = OFF;    /* Long Format flag  */
uint8_t Enable_Hidden_Files = OFF;    /* Hidden Files flag */
uint8_t Enable_Inode_Number = OFF;    /* Inode Flag */
uint8_t Enable_Sort	    = OFF;    /* Sort Flag */
uint8_t Enable_List_Dir	    = OFF;    /* List directory Flag*/
uint8_t Enable_List_OneLine = OFF;    /* List in one line flag */
Sorting_Option_t SortOption = Alph;   /* Sort Techniqe >> by default : based on the file name */

char SymLink_Path[1024] = {0};
uint8_t SymLinkFlag = OFF;
	
/* This array srores the months name */
const char *months[] = {
        "Jan"  , "Feb" , "Mar" , "Apr" ,
	"May"  , "Jun" , "Jul" , "Aug",
       	"Sept" , "Oct" , "Nov" , "Dec"
    };

/* This struct is so so important 
 * it has two structs 
 * (1) FileInfo : to store info get by readir function
 * (2) FileStat : to store info get by lstat function  
 * */
typedef struct {
        struct dirent FileInfo;
        struct stat   FileStat;
}ArrayOfInfo_t;


FileType_t FileType = REG;

/* we will have Array of the last struct to store the data first then apply sorting algorithm*/
ArrayOfInfo_t *ArrayOfInfo = NULL;
/* This variable will have the number of files (length of the last array)*/
static int FilesNum = 0;

extern errno;
/* #############################################################################
 * ################################# The Logic Of My Code :) ###################
 * #############################################################################
 * (1) Chaeck On the Options and set the flags according to the input option ###
 * (2) Open the directory and store the files data (dynamic allocation       ###
 * (3) Apply sorting Algorithm						     ###
 * (4) Print info according to the options 			             ###
 * #############################################################################
 * */
int main(int argc,char *argv[]){
     
   /* This function set the flags and return status , this status has 3 condions 
    * (1) OK       >> there is valid options 
    * (2) NOOPTION >> there is no options on the arguments {argv array}
    * (3) NOK      >> there is invalid options 
    * */
   stat_t RetStat = enmGetOptions(argc,argv);
	
   switch(RetStat){
   	case (NOK)      : printf("Unrecognized option : %c\n", optopt);    break; 
	case (NOOPTION) : do_simple_ls(argc,argv);                         break;
	case (OK)       : do_simple_ls((argc -optind +1),&argv[optind-1]); break;
   }
}





/* this fanction takes the arguments and extract the options at argument array
 * this function has three cases
 * (1) there is no options on the arguments {argv array} >> return "NOOPTION"
 * (2) there is valid options    >> return "OK"
 * (3) there is invalid options  >> return "NOK"
 * pram : argc : number of arguments 
 *        argv : array of string contain the argument 
 * ret  : RetStat : status of the function {mentioned above}
 * */
stat_t enmGetOptions(int argc,char *argv[]){
   /* default value of the "RetStat" is "NOOPTION" */
   stat_t RetStat = NOOPTION;

   opterr = 0;
   int opt = 0;
   struct option long_options[] = {
    {"color", no_argument, 0, 'm'},  // --color option
    {0, 0, 0, 0}                     // End of options
};
    while ((opt = getopt_long(argc, argv,
                  	     "latucifd1",
                  	     long_options,
		  	     NULL)) != -1){
        RetStat = OK;
	Argument_Option = ON;
	switch (opt) {
        case 'l': Enable_Long_Format  = ON; break;
        case 'a': Enable_Hidden_Files = ON; break;
        case 't': SortOption = modif_time ; break; 
        case 'u': SortOption = access_time; break;
        case 'c': SortOption = Inode_time ; break;
        case 'i': Enable_Inode_Number = ON; break;
        case 'f': SortOption = No_Sort    ;
		  Enable_Hidden_Files = ON; 
		  Enable_Long_Format  = OFF;break;
        case '1': Enable_List_OneLine = ON; break;
        case 'd': Enable_List_Dir     = ON; break;
        case '?': RetStat = NOK; 	    break;
	case 'm': Enable_Color	      = ON; break;	  
	default : printf("Unexpected case in switch()\n"); return -1;
        }
	if(NOK == RetStat) break;
   }
   return(RetStat); 
}

/* this function execute the ls function 
 * ex >> ./myls <options> <path 1> <path 2> ... <path n> 
 * pram : argc : number of arguments 
 *        argv : array of string contain the argument 
 * ret  : no return 
 * */
void do_simple_ls(int argc,char **argv){

   int cols      = 0; 	/* used for calculate the suitable number of outputs in one raw */
   int DirIndex  = 0;	/* iterate on all "directory pathes" on argv array */
   int FileIndex = 0;	/* iterate on all files on one directory */
   int MaxLen    = 0;	/* have the max length of file name on the directory to align with this length */
   
   /* This condition checks if the user enters only the program name
    * ex >> "myls" 
    * it will add to the argv array "." 
    * to make program excutes at the current directory 
    * */
   if (argc == 1){
      /* create string which has "."
       * and make argv[1] points to it 
       * */
      char *currentdir = "."; 
      argc = 2;
      argv[1] = currentdir;
   }

   /* loop on all input directory pathes */
   while(++DirIndex < argc){
	
	/* if the "-d" option is applied 
	 * > so we will also call "vdPrintDirInfo" function then terminate the program */
   	if(ON == Enable_List_Dir){
		vdPrintDirInfo(argv[DirIndex]);
		continue;
	}

	 /* get the max length of file name on the directory to align with this length*/
         MaxLen = GetMaxLen(argv[DirIndex]);
	 /* if MaxLen = -1 >> that means that it can't open this directory */
	 if(-1 == MaxLen){
		fprintf(stderr, "Cannot open directory : %s\n",argv[DirIndex]);
		continue;
	 }
	 /* get the suitable number of printed files on one raw */
	 cols   = GetTerminalLength(MaxLen);
   	 struct dirent * entry;     /* this struct will carry the file info */

   	 /* dp varible acts as file discriptor for the directory */
   	 DIR * dp = opendir(argv[DirIndex]);
	 struct stat statbuf;
	 /* incase of can't open the directory */
   	 if (dp == NULL){
      		fprintf(stderr, "Cannot open directory:%s\n",argv[DirIndex]);
		continue;
   	}
   	errno = 0;
	/* print the directory name
	 * ex >> " directory name : "
	 * */
	printf("%s : \n",argv[DirIndex]);
	/* start FileIndex from begain*/
	FileIndex = 0;
	int i =0;
	/* loop on all files at the directory */
   	while((entry = readdir(dp)) != NULL){
        	/* in case of failed readdir */
		if((entry == NULL)&&(errno != 0)){
                	perror("readdir failed");
			continue;
		}else{ 
			/* this part contain the full path of each file to call "lstat" function*/
			char filepath[1024] = {0};
			strcpy(filepath,argv[DirIndex]);
			lstat((strcat((strcat(filepath,"/")),entry->d_name)),&statbuf);

			/* store the two structs at the array of info */
			vdStoreTheDirContent(entry,&statbuf);
			memset(filepath, '\0', sizeof(filepath));
			errno = 0;

			strcpy(SymLink_Path ,argv[DirIndex]);
		}

   	}
   	closedir(dp);
	vdApplySorting();		/* Apply sorting algorithm on the data */
	vdPrintFiles(MaxLen,cols);	/* Print the Data */
	vdFreeTheDirContent(); 		/* Free The heap after printing */
   }
}

   

/* this function stores the two structs (readdie & lstat) on the heap 
 * pram : entry   : pointer to th struct produced by the readdir function
 *        statbuf : pointer to th struct produced by the lstat   function
 * ret  : no return 
 * */
void vdStoreTheDirContent(dir_t *entry,lstat_t *statbuf){
   /* Incase First Call (first allocation )*/
   if((0 == FilesNum)&&(NULL == ArrayOfInfo)){
	ArrayOfInfo  = (ArrayOfInfo_t *)malloc(sizeof(ArrayOfInfo_t));
	if(ArrayOfInfo == NULL){
	     	fprintf(stderr, "Error While Dynamic Allocation \n");
		exit(EXIT_FAILURE);
	}else{
		ArrayOfInfo[FilesNum].FileInfo   = *entry;	
		ArrayOfInfo[FilesNum++].FileStat = *statbuf;
	}
   }else{
	   	/* Incase it isn't first call */
	   	ArrayOfInfo  = (ArrayOfInfo_t *)realloc(ArrayOfInfo,(FilesNum+1)*(sizeof(ArrayOfInfo_t)));
		if(ArrayOfInfo == NULL){
                	fprintf(stderr, "Error While Dynamic Allocation \n");
                	exit(EXIT_FAILURE);
        	}else{
			ArrayOfInfo[FilesNum].FileInfo   = *entry;
			ArrayOfInfo[FilesNum++].FileStat = *statbuf;
        	}
   }
}

/* this function frees the allocated memory from the heap
 * param : no param 
 * ret   : no return
 * */
void vdFreeTheDirContent(){
   free(ArrayOfInfo);
   FilesNum = 0;
   ArrayOfInfo  = NULL;
}


/* this function Prints the info of the files according to the input options
 * pram : MaxLen  : this variable contain the max length of the names of the files 
 *        cols    : this variable contain the allowable columns at one raw   
 * ret  : no return 
 * */
void vdPrintFiles(int MaxLen, int cols){
   int FileIndex = 0;	/* Loop on the files */
   /* first check >> there is no option */
   if(OFF == Argument_Option){
	   /* Loop on the files locates at our array and print them */
	   for(int i = 0;i<FilesNum;i++){
		 /* Ignore the hidden files*/
	   	 if(ArrayOfInfo[i].FileInfo.d_name[0] == '.'){
		 	continue;
		 }else{
			/* print the content of the directory with alligned by MaxLen */
 			 printf("%-*s ", MaxLen, ArrayOfInfo[i].FileInfo.d_name);
        	        /* in case of reach to max allowable files on same raw */
                	if (++FileIndex % cols == 0) {
               	 	      printf("\n");  // Move to next row after filling a row
                	}
		 }
	   }
	   printf("\n");
   /* Incase Of there is options */	           
   }else{
	   /* loop */

	   cols = (Enable_Inode_Number == ON) ? cols/2  : cols ;
	    int p = 0; 		/* this variable hasn't any benifits */ 
	   for(int i = 0;i<FilesNum;i++){
		 /* if the hidden files option is enabled so it will be printed */
                 if((ArrayOfInfo[i].FileInfo.d_name[0]  == '.')&&(OFF == Enable_Hidden_Files)){
                       	continue;
               	}else{
			/* In case of long formate */
			if(ON == Enable_Long_Format){
				/* Show the inode number in case of its flag is on */
				(Enable_Inode_Number == ON) ? printf("%ld\t", ArrayOfInfo[i].FileStat.st_ino ) : p++;
				vdPrintPermissions(ArrayOfInfo[i].FileStat.st_mode);			
				printf("%ld\t", ArrayOfInfo[i].FileStat.st_nlink);
				vdPrintOwnerName(ArrayOfInfo[i].FileStat.st_uid);
				vdPrintGroupName(ArrayOfInfo[i].FileStat.st_gid);
				printf("%ld\t", ArrayOfInfo[i].FileStat.st_size);
				time_t secs = ArrayOfInfo[i].FileStat.st_mtim.tv_sec;
				struct tm *time_info = localtime(&secs);
				printf("%d:%d\t%d %s\t",time_info->tm_hour,time_info->tm_min,time_info->tm_mday,months[time_info->tm_mon]);
				(Enable_Color == ON) ? vdPrintColoredName(FileType,i,0):printf("%s",  ArrayOfInfo[i].FileInfo.d_name);
				vdPrintSymLinkPath(i);
				
				printf("\n");
			}else{
				/* In case of not long list */
				(Enable_Inode_Number == ON) ? printf("%-*ld ", MaxLen,ArrayOfInfo[i].FileStat.st_ino) : p++;
				/* print the content of the directory with alligned by MaxLen */
				(Enable_Color == ON) ? vdGetFileType(ArrayOfInfo[i].FileStat.st_mode) : p++;
				(Enable_Color == ON) ? vdPrintColoredName(FileType,i,MaxLen):printf("%-*s ", MaxLen, ArrayOfInfo[i].FileInfo.d_name);
				/* in case of reach to max allowable files on same raw */
				if ((++FileIndex % cols == 0)&&(Enable_List_OneLine == OFF)) {
					printf("\n");  // Move to next row after filling a row

				}else{ 
					if(ON == Enable_List_OneLine)
						printf("\n"); 
				}
				
			}	
		 	
		}
           	
	   }

	   ((Enable_Long_Format == ON)||(Enable_List_OneLine == ON)) ?p++:printf("\n");
   }
   
}


void vdPrintColoredName(FileType_t fileType, int i,int MaxLen) {
    switch (fileType) {
        case DIRC: // Directory (blue)
            printf("\033[1;34m%-*s\033[0m",MaxLen,  ArrayOfInfo[i].FileInfo.d_name);
            break;
        case REG: // Regular file (default color)
            printf("%-*s",MaxLen,ArrayOfInfo[i].FileInfo.d_name);
            break;
        case SYM: // Symbolic link (cyan)
            printf("\033[1;36m%-*s\033[0m", MaxLen, ArrayOfInfo[i].FileInfo.d_name);
            break;
        case BLK: // Block device (yellow)
            printf("\033[1;33m%-*s\033[0m",MaxLen, ArrayOfInfo[i].FileInfo.d_name);
            break;
        case CHR: // Character device (yellow)
            printf("\033[1;33m%-*s\033[0m",  MaxLen,ArrayOfInfo[i].FileInfo.d_name);
            break;
        case FFO: // FIFO (purple)
            printf("\033[1;35m%-*s\033[0m",MaxLen,  ArrayOfInfo[i].FileInfo.d_name);
            break;
        case SOC: // Socket (green)
            printf("\033[1;32m%-*s\033[0m",  MaxLen,ArrayOfInfo[i].FileInfo.d_name);
            break;
        default: // Unknown file type (default color)
            printf("%-*s",  MaxLen,ArrayOfInfo[i].FileInfo.d_name);
    }
}

void vdPrintSymLinkPath(int index){

	if(SymLinkFlag == ON ){
		ssize_t bufsize =  readlink((strcat( (strcat(SymLink_Path,"/")),( ArrayOfInfo[index].FileInfo.d_name))),
				   SymLink_Path,
				   1024);

		(Enable_Color == ON) ? printf(" -> \033[1;34m%s\033[0m\t",SymLink_Path):printf(" -> %s\t",SymLink_Path);
		memset(SymLink_Path,'\0',1024);
	}
}


/* this function Prints the info of the directories incase of -d option
 * pram : DirPath  : path of the input directory  
 * ret  : no return 
 * */
void vdPrintDirInfo(char *DirPath){
   /* the flow of tthis function is as above but it isn't applied at the array */
   struct stat statbuf;
   int p = 0;
   static int FileIndex = 0;
   if((lstat(DirPath,&statbuf)) == -1){
   	fprintf(stderr, "Can't Access : %s\n",DirPath);
	return;
   }
   
   if(ON == Enable_Long_Format){   
	   (Enable_Inode_Number == ON) ? printf("%ld\t", statbuf.st_ino ) : p++;

	   vdPrintPermissions(statbuf.st_mode);
           printf("%ld\t", statbuf.st_nlink);
           vdPrintOwnerName(statbuf.st_uid);
           vdPrintGroupName(statbuf.st_gid);
           printf("%ld\t", statbuf.st_size);
           time_t secs = statbuf.st_mtim.tv_sec;
           struct tm *time_info = localtime(&secs);
           printf("%d:%d\t%d %s\t",time_info->tm_hour,time_info->tm_min,time_info->tm_mday,months[time_info->tm_mon]);
           printf("\033[1;34m%s\033[0m", DirPath);


          printf("\n");           
   }else{                              
	   (Enable_Inode_Number == ON) ? printf("%ld\t ",statbuf.st_ino ) : p++;                     
	   /* print the content of the directory with alligned by MaxLen */                             
//	   printf("%s\t",DirPath);                            
	   printf("\033[1;34m%s\033[0m\t", DirPath);
	   /* in case of reach to max allowable files on same raw */                            
	   if ((++FileIndex % (4) == 0)&&(Enable_List_OneLine == OFF)) {                             
		   printf("\n");  // Move to next row after filling a row                            
	   }else{                            
		   if(ON == Enable_List_OneLine)                 
			   printf("\n");                             
	   }

   }
}

/* this function Prints the PrintPermissions and type of the file 
 * pram : mode : this number contain the permisions and the type on octal form  
 * ret  : no return
 * */
void vdPrintPermissions(int  mode){
   SymLinkFlag = OFF;
   char str[11];
   strcpy(str, "----------");

   // mode struct 
   // ###########################################################################################
   // ############################## TYPE | SPECIAL | OWNER | GROUP | OTHER #####################
   // ## number of octal digits ####  2   |    1    |   1   |   1   |   1   #####################
   // ###########################################################################################

   //owner  permissions # r w x #
   if((mode & 0000400) == 0000400) str[1] = 'r'; /* 4 >> 100 */
   if((mode & 0000200) == 0000200) str[2] = 'w'; /* 2 >> 010 */
   if((mode & 0000100) == 0000100) str[3] = 'x'; /* 1 >> 001 */
   //group permissions
   if((mode & 0000040) == 0000040) str[4] = 'r';
   if((mode & 0000020) == 0000020) str[5] = 'w';
   if((mode & 0000010) == 0000010) str[6] = 'x';
   //others  permissions
   if((mode & 0000004) == 0000004) str[7] = 'r';
   if((mode & 0000002) == 0000002) str[8] = 'w';
   if((mode & 0000001) == 0000001) str[9] = 'x';
   //special  permissions
   if((mode & 0004000) == 0004000) str[3] = 's';
   if((mode & 0002000) == 0002000) str[6] = 's';
   if((mode & 0001000) == 0001000) str[9] = 't';

   // 0170000 is bitmask from the man inode
   // Type 
   if ((mode &  0170000) == 0010000) 		{str[0] = 'p'; FileType = FFO;} 
   else if ((mode &  0170000) == 0020000)	{str[0] = 'c'; FileType = CHR;}
   else if ((mode &  0170000) == 0040000)	{str[0] = 'd'; FileType = DIRC;}
   else if ((mode &  0170000) == 0060000)	{str[0] = 'b'; FileType = BLK;}
   else if ((mode &  0170000) == 0100000)	{str[0] = '-'; FileType = REG;}
   else if ((mode &  0170000) == 0120000)	{str[0] = 'l'; SymLinkFlag = ON; FileType = SYM;}
   else if ((mode &  0170000) == 0140000)	{str[0] = 's'; FileType = SOC;}


   printf("%s  ", str);

}

void vdGetFileType(int mode){
 if ((mode &  0170000) == 0010000)            	{FileType = FFO;}
   else if ((mode &  0170000) == 0020000)       {FileType = CHR;}
   else if ((mode &  0170000) == 0040000)       {FileType = DIRC;}
   else if ((mode &  0170000) == 0060000)       {FileType = BLK;}
   else if ((mode &  0170000) == 0100000)       {FileType = REG;}
   else if ((mode &  0170000) == 0120000)       {FileType = SYM;}
   else if ((mode &  0170000) == 0140000)       {FileType = SOC;}
}

/* The Next 4 functions are different sorting algorithms */
static int intSortBasedOnAlpha (const void *p1, const void *p2)
{
    ArrayOfInfo_t *File1 = (ArrayOfInfo_t *)p1;
    ArrayOfInfo_t *File2 = (ArrayOfInfo_t *)p2;

    return strcmp(File1->FileInfo.d_name,File2->FileInfo.d_name);
}
static int intSortBasedOnAccessT (const void *p1, const void *p2)
{
    ArrayOfInfo_t *File1 = (ArrayOfInfo_t *)p1;
    ArrayOfInfo_t *File2 = (ArrayOfInfo_t *)p2;

    return ((File2->FileStat.st_atim.tv_sec) - (File1->FileStat.st_atim.tv_sec ));
}

static int intSortBasedOnModifyT (const void *p1, const void *p2)
{
    ArrayOfInfo_t  *File1 = (ArrayOfInfo_t *)p1;
    ArrayOfInfo_t  *File2 = (ArrayOfInfo_t *)p2;

    return ((File2->FileStat.st_mtim.tv_sec) - (File1->FileStat.st_mtim.tv_sec ));
}

static int intSortBasedOnInodeT (const void *p1, const void *p2)
{
    ArrayOfInfo_t *File1 = (ArrayOfInfo_t *)p1;
    ArrayOfInfo_t *File2 = (ArrayOfInfo_t *)p2;

    return ((File2->FileStat.st_ctim.tv_sec) - (File1->FileStat.st_ctim.tv_sec ));
}



/* this function sorts the array based on the input options
 * param : no param 
 * ret   : no return
 * */
void vdApplySorting(){

   switch (SortOption){
	   case (Alph)       : qsort(ArrayOfInfo, FilesNum , sizeof(ArrayOfInfo_t), intSortBasedOnAlpha);  break;
 	   case (No_Sort)    :										   break;
	   case (modif_time) : qsort(ArrayOfInfo, FilesNum , sizeof(ArrayOfInfo_t), intSortBasedOnModifyT);break;
	   case (access_time): qsort(ArrayOfInfo, FilesNum , sizeof(ArrayOfInfo_t), intSortBasedOnAccessT);break;
           case (Inode_time) : qsort(ArrayOfInfo, FilesNum , sizeof(ArrayOfInfo_t), intSortBasedOnInodeT); break;
   }
}


/* this function prints the name of the user given his id 
 * param : uid : User id 
 * ret   : no return
 * */
void vdPrintOwnerName(int uid){
   const struct passwd * PasswdStruct  = getpwuid(uid);
   if (PasswdStruct == NULL){
	printf("%d\t",uid);
   }else{
	printf("%s\t",PasswdStruct->pw_name);
   }
}


/* this function prints the name of the group given his id
 * param : gid : Group id
 * ret   : no return
 * */
void vdPrintGroupName(int gid){
   struct group * grp = getgrgid(gid);
   if (grp == NULL){
        printf("%d\t",gid);
   }else{
        printf("%s\t",grp->gr_name);
   }
}



/* this function calculates the max length of the files on the directory 
 * ex >> {file1 , file1234 , file} >> max_len = LengthOf(file1234) = 8
 * pram : DirName : the path of the directory 
 * ret  : max_len
 * */
int GetMaxLen(char *DirName){
    
    struct dirent *entry; 		/* this struct will contain the info of each file at the directory */
    DIR *dp = opendir(DirName);		/* file discriptor of the directory */

    /* error when open the directory */
    if (dp == NULL) {
        perror("opendir");
	/* return -1 incase of failed to open*/
        return(-1);
    }

    // Get the maximum length of the filenames
    int max_len = 0;
    int len  = 0;
    /* iterate on all files at the directory */
    while ((entry = readdir(dp))) {
        len = strlen(entry->d_name);
        if (len > max_len) {
            max_len = len;
        }
    }
    closedir(dp);
    return(max_len);
}

/* this function calculate the suitable number of files which printed on one row
 * it takes into concidration the terminal length  
 * pram : MaxLen : MaxLen which calculated in "GetMaxLen" function 
 * ret  : suitable number of files which printed on one row
 * {to be honst >> i get it from chatgpt :) }
 * */
int GetTerminalLength(int MaxLen){
   struct winsize w;
   ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
   int terminal_width = w.ws_col;
   return (terminal_width / (MaxLen + 3));
}
