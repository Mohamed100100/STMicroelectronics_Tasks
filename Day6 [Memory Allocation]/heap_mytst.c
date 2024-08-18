#include <stdio.h>
//#include <stddef.h>
//#include <stdlib.h>
#include "heap.h"

void *ptr = NULL;

int main()
{
	int var = 0;
	void *ptr1 = NULL;
	int choice = 0;
	while(1)
	{
                printf("======================================\n");		
		printf("1- for malloc  \n");
        	printf("2- for free  \n");
		printf("3- Display Free Nodes info\n");
        	printf("4- Display The Program Break Address\n");
                printf(" Enter Your Choice :");
	        scanf("%d", &choice);
 		printf("======================================\n");

		if(choice == 1)
		{
	    		printf(" Enter The Number of Bytes :");
			scanf("%d", &var);
			printf("======================================\n");
			ptr = HmmAlloc(var);
	                printf("======================================\n");
			printf("Allocated Ponter : %p\n",ptr);
	                printf("======================================\n");

		}
		if( choice == 2)
		{			 
     			printf(" Enter The Free Ptr :");
			scanf("%p", &ptr1);
	                printf("======================================\n");
			ptr = HmmFree(ptr1);
                        printf("======================================\n");
			printf("Free Pointer : %p\n",ptr);
     			printf("======================================\n");
		}	
		if(choice == 3)
		{
			printf("======================================\n");
			vdDisplayFreeNode();
			printf("======================================\n");

		}
		if(choice == 4)
		{
			printf("======================================\n");
			vdDisplayPrgBrk();
			printf("======================================\n");
		}
	}
}
