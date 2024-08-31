#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <inttypes.h>
#include <string.h>

/* Sector size */
#define SECTOR_SIZE 	(512)

// GPT Partition Types (Famous Ones)
#define GPT_PARTITION_TYPE_UNUSED              "00000000-0000-0000-0000-000000000000"
#define GPT_PARTITION_TYPE_EFI_SYSTEM          "C12A7328-F81F-11D2-BA4B-00A0C93EC93B"
#define GPT_PARTITION_TYPE_BIOS_BOOT           "21686148-6449-6E6F-744E-656564454649"
#define GPT_PARTITION_TYPE_MICROSOFT_RESERVED  "E3C9E316-0B5C-4DB8-817D-F92DF00215AE"
#define GPT_PARTITION_TYPE_MICROSOFT_BASIC_DATA "EBD0A0A2-B9E5-4433-87C0-68B6B72699C7"
#define GPT_PARTITION_TYPE_LINUX_FILESYSTEM    "0FC63DAF-8483-4772-8E79-3D69D8477DE4"
#define GPT_PARTITION_TYPE_LINUX_SWAP          "0657FD6D-A4AB-43C4-84E5-0933C84B4F4F"
#define GPT_PARTITION_TYPE_LINUX_LVM           "E6D6D379-F507-44C2-A23C-238F2A3DF928"
#define GPT_PARTITION_TYPE_APPLE_HFS           "48465300-0000-11AA-AA11-00306543ECAC"
#define GPT_PARTITION_TYPE_APPLE_APFS          "7C3457EF-0000-11AA-AA11-00306543ECAC"
#define GPT_PARTITION_TYPE_SOLARIS_BOOT        "6A82CB45-1DD2-11B2-99A6-080020736631"
#define GPT_PARTITION_TYPE_FREEBSD_UFS         "516E7CB4-6ECF-11D6-8FF8-00022D09712B"
#define GPT_PARTITION_TYPE_FREEBSD_SWAP        "516E7CB6-6ECF-11D6-8FF8-00022D09712B"
#define GPT_PARTITION_TYPE_FREEBSD_VINUM       "516E7CB8-6ECF-11D6-8FF8-00022D09712B"
#define GPT_PARTITION_TYPE_FREEBSD_ZFS         "516E7CBA-6ECF-11D6-8FF8-00022D09712B"
#define GPT_PARTITION_TYPE_ANDROID_BOOTLOADER  "2568845D-2332-4676-BC39-8FA5A4748D15"
#define GPT_PARTITION_TYPE_ANDROID_SYSTEM      "114EAFFE-1552-4022-B26E-9B053604CF84"
#define GPT_PARTITION_TYPE_ANDROID_DATA        "A13B4F4D-4451-11E3-9D5D-0002A5D5C51B"

// MBR Partition Types (Famous Ones)
#define LINUX_TYPE      (0x83)
#define EXTENDED_TYPE   (0x05)
#define GPT_TYPE        (0xEE)

// unit values
#define GIGA_CHECK      (1024*1024*1024)
#define MIGA_CHECK      (1024*1024)
#define KILO_CHECK      (1024)

/* store the char of each unit {giga and mega and ,...} and also stor 
 * the value to do our calculations on the pragram  */
static const char     Units_Symbol[]  = {'G','M','K'};
static const uint64_t Units_Value[] = {GIGA_CHECK,MIGA_CHECK,KILO_CHECK};

/* Partaions MBR Type */
static const char *TypeName[]   = {"Linux","Extended","Unknown"};
/* Partaions GPT Type */
static const char *TypeGPTName[]   = {"UNUSED","EFI_SYSTEM","BIOS_BOOT"
				     ,"MICROSOFT_RESERVED","MICROSOFT_BASIC_DATA",
				      "LINUX_FILESYSTEM","LINUX_SWAP","LINUX_LVM",
				      "APPLE_HFS","APPLE_APFS","SOLARIS_BOOT","FREEBSD_UFS",
				      "FREEBSD_SWAP","FREEBSD_VINUM","FREEBSD_ZFS",
				      "ANDROID_BOOTLOADER","ANDROID_SYSTEM","ANDROID_DATA","UNKNOWN"};

/* Partaions GPT Values */
static const char *TypeGPTValue[]  ={GPT_PARTITION_TYPE_UNUSED,GPT_PARTITION_TYPE_EFI_SYSTEM,
				     GPT_PARTITION_TYPE_BIOS_BOOT,GPT_PARTITION_TYPE_MICROSOFT_RESERVED,
				     GPT_PARTITION_TYPE_MICROSOFT_BASIC_DATA,GPT_PARTITION_TYPE_LINUX_FILESYSTEM,
				     GPT_PARTITION_TYPE_LINUX_SWAP,GPT_PARTITION_TYPE_LINUX_LVM,GPT_PARTITION_TYPE_APPLE_HFS,
				     GPT_PARTITION_TYPE_APPLE_APFS,GPT_PARTITION_TYPE_SOLARIS_BOOT,GPT_PARTITION_TYPE_FREEBSD_UFS,
				     GPT_PARTITION_TYPE_FREEBSD_SWAP,GPT_PARTITION_TYPE_FREEBSD_VINUM,GPT_PARTITION_TYPE_FREEBSD_ZFS,
				     GPT_PARTITION_TYPE_ANDROID_BOOTLOADER,GPT_PARTITION_TYPE_ANDROID_SYSTEM,GPT_PARTITION_TYPE_ANDROID_DATA};


// GPT Partition Entry structure
typedef struct{
    uint8_t partition_type_guid[16];   // Partition type GUID
    uint8_t unique_partition_guid[16]; // Unique partition GUID
    uint64_t first_lba;                // First LBA of the partition
    uint64_t last_lba;                 // Last LBA of the partition
    uint64_t attributes;               // Partition attributes
    uint16_t partition_name[36];       // Partition name (Unicode UTF-16)
}gpt_partition_entry;

// MBR Partition Entry structure
typedef struct {
    uint8_t status;
    uint8_t first_chs[3];
    uint8_t partition_type;
    uint8_t last_chs[3];
    uint32_t lba;
    uint32_t sector_count;
} PartitionEntry;

uint32_t ExtendStartSector = 0;

/* ProtoTypes of helper functions */
void vdPrintPrimeContent(uint8_t i , PartitionEntry *table_entry_ptr,char *PartationName);
void vdPrintExtendedContent(uint8_t i , PartitionEntry *table_entry_ptr,char *PartationName);
uint8_t  u8KnowTheType(uint8_t PartitionType);
uint8_t  u8KnowSuitableChar(uint32_t SectorNum);
uint8_t  u8KnowTheTypeOfGPT(uint8_t *partition_type_guid);
void vdPrintGPTContent(uint8_t i, gpt_partition_entry *gpt_partition_entry_ptr,char *PartationName);

int main(int argc, char **argv)
{
    char buf[512];
    // open file which has the info about the selected hard
    // for ex >> "sda" OR "sdb"
    int fd = open(argv[1], O_RDONLY);
    if(fd==-1){
	    printf("error : can't open this file\n");
	    return 1;
	}

   // read the first sector of the file {which has the MBR header}
    if((read(fd, buf, 512))==-1){
	printf("error : can't read from the file\n");
	return 1;
    }

    // read only the last 66 byte and ignore the boot part 
    PartitionEntry *table_entry_ptr = (PartitionEntry *) & buf[446];
    // check if the type is GPT
   if((table_entry_ptr->partition_type) == GPT_TYPE){
	   
	   printf("Disklabel type: GPT\n");
	   printf("%-10s %-10s %-10s %-10s %-10s %-8s\n", "Device",
            "Start", "End", "Sectors", "Size", "   Type");
		
	   /* Move to the start of the second sector*/
	   gpt_partition_entry *gpt_partition_entry_ptr =NULL;
	if(lseek(fd,2*SECTOR_SIZE, SEEK_SET)!=-1){
		uint8_t index = 0;
		/* Loop on the info of all partaions and stop if the next partation has start = 0 */
		do{
			/* Read the info of the parthion #i */
             		if((read(fd, buf, sizeof(gpt_partition_entry)))!=-1){
		
				// put the read data at the struct of the GPT info
				gpt_partition_entry_ptr = (gpt_partition_entry *)buf;
				/* Print the info of the partaion #i */
				vdPrintGPTContent(index++,gpt_partition_entry_ptr,argv[1]);
			}
			/* Unable To open the file */
			else{
				printf("error : can't read from the file\n");
				return 1;
			}
		/* Loop till the start sector = 0*/
		}while(gpt_partition_entry_ptr->first_lba !=0);
	/* Unable to seek at the file */
	}else{
		 printf("error : can't seek at the file\n");
                 return 1;
	}	
	
   /* The kind is MBR */
   }else{		            

	   printf("Disklabel type: MBR\n");
     	   printf("%-10s %-10s %-10s %-10s %-10s %-10s %-10s %-8s\n", "Device",
	   "Boot", "Start", "End", "Sectors", "Size", "Id", "Type");

	   /* Loop 4 times but ignore when the start sector =0 */
    	   for (int i = 0; i < 4; i++) {

		/* Print the info about the #i partation*/
		vdPrintPrimeContent(i , table_entry_ptr,argv[1]);		
 
		/* store the start sector of the extended partation if exists*/
		if((table_entry_ptr[i].partition_type)==EXTENDED_TYPE){
			ExtendStartSector = table_entry_ptr[i].lba;
		}
    	}

	/* the next part ,we will print the extended partaions */
   	uint8_t ExtendIndex = 5;
	/* we will update this value with the start of the next extended partaion */
    	while(ExtendStartSector!=0){
    	
		/* move to the start of the sector of extended partaion#i*/
		if(lseek(fd,ExtendStartSector*SECTOR_SIZE, SEEK_SET)!=-1){
			/* read the sector */
			if((read(fd, buf, 512))!=-1){
				/* store the last 66 byte for the info only and ignore the boot */
  				table_entry_ptr = (PartitionEntry *) & buf[446];
				vdPrintExtendedContent(ExtendIndex++ , table_entry_ptr,argv[1]);
 			/* Unable to read from the file */
			}else{
				printf("error : can't read from the file\n");
		        	return 1;
			}
		/* Unable to seek at the file */
		}else{
			printf("error : can't seek at the file\n");
       			return 1;
		}
    	}
   }
  
    return 0 ;
}


/* this function know the sutable char of the size
 * ex >> if the size is kilo it will return index of this char at the "Units_Symbol" array 
 * it also helps at know if it is suitable to divide on (1024 * 1024 *1024) OR (1024 * 1024)
 * pram : SectorNum
 * ret  : the index of the Units_Symbol array
 **/
uint8_t u8KnowSuitableChar(uint32_t SectorNum)
{
	/* calculate the size on bytes */
	uint64_t NumOfBytes = SectorNum*SECTOR_SIZE;
	if((NumOfBytes/GIGA_CHECK) > 0)
	{
		return (0);
	}
	else{ 
		if((NumOfBytes/MIGA_CHECK) > 0){
			return(1);
	}
		else{
			if((NumOfBytes/KILO_CHECK) > 0){
			
				return(2);
			}
		}
	}
}

/* this function get the type of the MBR disk
 * it return an index to the "TypeName" array according to the type
 * param : PartitionType
 * ret   : index 
 * */
uint8_t u8KnowTheType(uint8_t PartitionType){
	/* check on all MBR disk types */
	uint8_t RetIndex = 0;
	switch(PartitionType)
	{
		case (LINUX_TYPE)    : RetIndex = 0;break;
		case (EXTENDED_TYPE) : RetIndex = 1;break; 
		default              : RetIndex=2; break; 
	}

return RetIndex;
}

/* this function prints the info about the main 4 partaions of the MBR disk
 * param : index of the partaion 
 * param : struct of the info about the partaion 
 * param : partaion name ex >> "sda" OR "sdb"
 * */
void vdPrintPrimeContent(uint8_t i , PartitionEntry *table_entry_ptr,char *PartationName){
   if(table_entry_ptr[i].lba != 0){
  	 uint8_t IndexOfUnit = u8KnowSuitableChar(table_entry_ptr[i].sector_count);
         uint8_t IndexOfType = u8KnowTheType(table_entry_ptr[i].partition_type);


         printf("%s%-3d %-9c %-10u %-10u %-10u %u%c \t  %-10X %s \n",
                 PartationName,
                 i + 1,
                 table_entry_ptr[i].status == 0x80 ? '*' : ' ',
                 table_entry_ptr[i].lba,
                 table_entry_ptr[i].lba + table_entry_ptr[i].sector_count - 1,
                 table_entry_ptr[i].sector_count,
                 ((uint32_t) (((uint64_t) table_entry_ptr[i].sector_count *512) / (Units_Value[IndexOfUnit]))),
                 Units_Symbol[IndexOfUnit],
                 table_entry_ptr[i].partition_type,
                 TypeName[IndexOfType]);
  }else{
       	/* nothing */
  }
}

/*this function prints the info about the main Extended partaions of the MBR disk
* param : index of the partaion
* param : struct of the info about the partaion
* param : partaion name ex >> "sda" OR "sdb"
* */

void vdPrintExtendedContent(uint8_t i , PartitionEntry *table_entry_ptr,char *PartationName){
    if(table_entry_ptr->lba != 0){
          uint8_t IndexOfUnit = u8KnowSuitableChar(table_entry_ptr->sector_count);
          uint8_t IndexOfType = u8KnowTheType(table_entry_ptr->partition_type);
          printf("%s%-3d %-9c %-10u %-10u %-10u %u%c \t  %-10X %s \n",
		  PartationName,
                  i ,
                  table_entry_ptr->status == 0x80 ? '*' : ' ',
                  (table_entry_ptr->lba)+ExtendStartSector,
                  table_entry_ptr->lba + ExtendStartSector+table_entry_ptr->sector_count - 1,
                  table_entry_ptr->sector_count,
                  ((uint32_t) (((uint64_t) table_entry_ptr->sector_count *512) / (Units_Value[IndexOfUnit]))),
                  Units_Symbol[IndexOfUnit],
                  table_entry_ptr->partition_type,
                  TypeName[IndexOfType]);
	  		
	  	  /* Update this variable to the start of the next sector*/
	          ExtendStartSector =table_entry_ptr->lba + ExtendStartSector+table_entry_ptr->sector_count;

   }else{
	   ExtendStartSector = 0;
   }
 }


/* this function prints the info about the main Extended partaions of the GPT disk
 * param : index of the partaion
 * param : struct of the info about the partaion
 * param : partaion name ex >> "sda" OR "sdb"
 **/
void vdPrintGPTContent(uint8_t i, gpt_partition_entry *gpt_partition_entry_ptr,char *PartationName){
    if(gpt_partition_entry_ptr->first_lba != 0){
    	
	    uint8_t IndexOfUnit = u8KnowSuitableChar(gpt_partition_entry_ptr->last_lba -gpt_partition_entry_ptr->first_lba);
	    uint8_t IndexOfType = u8KnowTheTypeOfGPT(gpt_partition_entry_ptr->partition_type_guid);
    
	printf("%s%-2d %-10lu %-10lu %-10lu %u%c \t  %s \n",
        	PartationName,
            	i + 1,
            	gpt_partition_entry_ptr->first_lba,
            	gpt_partition_entry_ptr->last_lba,
            	(gpt_partition_entry_ptr->last_lba -gpt_partition_entry_ptr->first_lba+1),
            	((uint32_t) (((uint64_t) (gpt_partition_entry_ptr->last_lba -gpt_partition_entry_ptr->first_lba+1)*512) / (Units_Value[IndexOfUnit]))),
            	Units_Symbol[IndexOfUnit],
            	TypeGPTName[IndexOfType]);
  }else{
        /* nothing */
   }

}
        

/* this function get the type of the GPT disk
* it return an index to the "TypeGPTName" array according to the type
* param : PartitionType
* ret   : index 
* */
uint8_t u8KnowTheTypeOfGPT(uint8_t *partition_type_guid){
	
	/* Store the partaion name at this array*/	
	char guid_str[30] ;
	/* This print will calculate the HEXA Type to string to be compared by the stored type array*/
	snprintf(guid_str, 37,
             "%02X%02X%02X%02X-%02X%02X-%02X%02X-%02X%02X-%02X%02X%02X%02X%02X%02X",
             partition_type_guid[3] , partition_type_guid[2] , partition_type_guid[1] , partition_type_guid[0] ,
             partition_type_guid[5] , partition_type_guid[4] , partition_type_guid[7] , partition_type_guid[6] ,
             partition_type_guid[8] , partition_type_guid[9] , partition_type_guid[10], partition_type_guid[11], 
	     partition_type_guid[12], partition_type_guid[13], partition_type_guid[14], partition_type_guid[15]);	
	
	for(uint8_t i =0;i<18;i++){

		if((strcmp(guid_str,TypeGPTValue[i]))==0){
			return(i);
		}
	}
	/* if we reach here , the input type is not stored at the program*/
	return(18);
}
