


#define NO_OPTION       (-3)
#define ON	        (0)
#define OFF	        (1)
typedef enum {
 OK,
 NOOPTION,
 NOK
}stat_t;

typedef enum {
 DIRC,
 REG,
 SYM,
 BLK,
 CHR,
 FFO,
 SOC
}FileType_t;

typedef enum {
 No_Sort,
 modif_time,
 access_time,
 Inode_time,
 Alph,
}Sorting_Option_t;

typedef struct dirent dir_t;
typedef struct stat   lstat_t;
int GetMaxLen(char *DirName);
void do_simple_ls(int argc,char **argv);
stat_t enmGetOptions(int argc,char *argv[]);
int GetTerminalLength(int MaxLen);
void vdStoreTheDirContent(dir_t *entry,lstat_t *statbuf);
void vdPrintFiles(int MaxLen, int cols);
void vdFreeTheDirContent();
void vdPrintPermissions(int mode);
void vdPrintOwnerName(int uid);
void vdPrintGroupName(int gid);
void vdPrintDirInfo(char *DirPath);
void vdApplySorting();
static int intSortBasedOnAlpha (const void *p1, const void *p2);
static int intSortBasedOnAccessT (const void *p1, const void *p2);
static int intSortBasedOnModifyT (const void *p1, const void *p2);
static int intSortBasedOnInodeT (const void *p1, const void *p2);
void vdPrintSymLinkPath(int index);
void vdPrintSumBlocks();
void vdPrintColoredName(FileType_t fileType, int i,int MaxLen);
void vdGetFileType(int mode);

