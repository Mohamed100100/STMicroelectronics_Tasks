#include <stdio.h>
//#include <stdlib.h>

#include "heap.h"

// this array will simulate the heap
// I make it char type to make each cell = one byte
// you can change its size by editing "HEAP_SIZE" Macro at "heap.h"
static char Heap_arr[HEAP_SIZE] = {0};

// The "Prog_Brk" points to the final of the heap (Virtually)
// The "HeapEnd"  points to the final of the heap (Physically)
//
static void *Heap_Start = &Heap_arr[0];
static void *Prog_Brk = &Heap_arr[0];
static void *HeapEnd  = &Heap_arr[HEAP_SIZE-1];

// this struct will defined to contain the free node metadata
// so the size of the metadata of the free node is 24 Bytes
struct node{
	size_t 		Blk_size;
	struct node*    next;
	struct node*    prev;
} ;

// this nodes are important for the free nodes of the linkedlist
// the "head" points to the first free node
// the "tail" points to the last  free node
struct node *head = NULL;
struct node *tail = NULL;

// I added to the "heap.h" macro called "DEBUG"
// and i added to each case of the program "printf" statments to test my code
// if you want to enable it put at it "ON" and if you don't put "OFF"


// ##########################################################################################################
// #################################### Important Info ######################################################
// >>> we will set the size of the heap (declared as macro) so you can change it(ex = 1000 byte) "HEAP_SIZE"
// >>> we will set the page size (declared as macro) so you can change it(ex = 100 byte) "PAGE_SIZE"
// >>> NOTE >> the "Prog_Brk" move with number which is multiple of the "PAGE_SIZE" (ex = 1 page or 2 page and so on) 
// >>> the metadata for each free node equal 24 byte(the size variable (8 byte) + 
//     two pointers one points to the next free node and another for the previous)
// >>> the metadata for the allocated node will equal 8 byte (the size variable only)
// ##########################################################################################################
// ##########################################################################################################

// ################################## Cases for the HmmALLOC() FUNC ######################################### 
// Case_1 : the program break at the start of the heap (first malloc call)
// challenge : how many pages will the program break move by ??
// IMPORTANT NOTE >> the "Prog_Brk" move with number which is multiple of the "PAGE_SIZE"
// MY SOLVE >> I will move it by the user input size + the ALLOCATED meatadata + FREE metadata 
// >> so if  user wants to allocate 90 byte >> so {90 + 8 + 24 = 122 Byte } i will move it by {200 byte} 2 "PAGE_SIZE"
// so we must add the free metadata(24 byte) and the allocated metadata(8 byte) also
// ------------------------------------------------------------------------------------------------------------
// Case_2 : There is already free nodes
// so we have has three sub_cases
// (1) we find a free node which its size equal to the input size (Happy Case)
// (2) we find a free node which its size is grater than the input size (search on the small_suitable_size)
//     >> at this case we have two sub_cases
//     >> (A) the difference between them(small_suitable_size and the input size) is grater than the free metadata
//        so we will give the user its space and record the other in the free node 
//        (B) the difference between them(small_suitable_size and the input size) is less than the free metadata
//        so we will give the user all free spaces including the difference(because we can't put them at free node
// (3) all free node has size smaller than the input size or there isn't free node (we must move ProgBrk)
// -------------------------------------------------------------------------------------------------------------

void *HmmAlloc(size_t size)
{
	if(size <=0)
	{
		return NULL;
	}
	// "retPtr" will be used to have the return pointer
	void *retPtr = NULL;

	// "temp" will be used in many things in allocating
	void *temp = NULL;

	// handle Case_1
	//start of the heap(first call of the malloc)
	if(Prog_Brk == &Heap_arr[0])
	{
		// check if the heap size is not suitable for the user to allocate 
		if((Prog_Brk + ((int)((size+ALL_METADATA)/PAGE_SIZE)+1)*PAGE_SIZE) >= HeapEnd)
		{
			return NULL;
		}

		// make this pointer points to the start of the heap
		temp = Prog_Brk;
				
#if(DEBUG == ON)
printf("First Call malloc \n");
printf("Prog_Brk(initially) = %p \n",Prog_Brk);
#endif
		// move the program break by the suitable page numbers according to the first explaination
		Prog_Brk = Prog_Brk + ((int)((size+ALL_METADATA)/PAGE_SIZE)+1)*PAGE_SIZE;
#if(DEBUG == ON)
printf("Suitable number of pages = %d\n",((int)((size+ALL_METADATA)/PAGE_SIZE)+1));
printf("Prog_Brk(finally) = %p \n",Prog_Brk);
#endif
			
		// cast it to the size_varible and put at it the user need size
		*((size_t *)(temp)) = size;
		// move this pointer to the next byte of the
		// temp   --->|     size     | 8 byte
		// retPtr --->|return pointer| void ptr
		retPtr = (void *)((void *)temp + ALLOC_METADATA);
#if(DEBUG == ON)
printf("Ret Ptr = %p \n",retPtr);
printf("allocated size = %ld \n",*((size_t *)(temp)));
#endif
			
		// move the temp to the next un used (will be free) bytes 
		// then make the head points to it then cast it to free node struct
		//        --->|     size     | 8 byte
                // retPtr --->|return pointer| void ptr
		// 	      |     ....     |
		// 	      |	    ....     |
		// head --->  |   free size  | >> #pages -input_size -2 * ALLOC_METADATA(8 byte)
		// 	      |   next free  | >> NULL
		// 	      |   prev free  | >> NULL
		temp = retPtr + size;
		head = (struct node *)temp;
		tail = head;

		head->Blk_size =  ((int)((size+ALL_METADATA)/PAGE_SIZE)+1)*PAGE_SIZE - size - 2*ALLOC_METADATA;
		head->next = NULL;
		head->prev = NULL;
#if(DEBUG == ON)
printf("Pointer to the next free area = %p\n",temp);
printf("Next free size %ld \n", head->Blk_size);
#endif
	
	}
	// malloc was called arleady
	else{
		// now we will split our work into two cases
		// first case  >> search for the free node which its size is equal to the input size "HAPPY CASE"
		// second case >> search for the free node which its size is grater than the input size
		// 	       	  But i search on the best one (smallest size but it must be grater than the input size)
		// third  case >> if the size of all free nodes is smaller than the input user space >> so move the program break pointer
		// 	       >> the steps will be the same as move the program break pointer for the first time

		// make the temp to loop on the free nodes
		struct node *temp = head ;
		struct node *ptr = NULL;
		
		// "small_suitable_size" variable will be used to get the best suitable size of the free node if it exists
		// i want to assign this variable to contain the max value which it can carry it 
		// i assign it to "-1" because the "size_t" type contain only +ve numbers 
		// so if i put at it "-1 " the variable will contain "all ones" and become have the max value of this type
		size_t small_suitable_size = -1;

		// this flag has three cases
		// flag = 1 >> we find a free node which its size equal to the input size (Happy Case)
		// flag = 2 >> we find a free node which its size is grater than the input size (search on the small_suitable_size)
		// 	    >> at this case we have two sub_cases
		// 	    >> (A) the difference between them(small_suitable_size and the input size) is grater than the free metadata
		// 	       so we will give the user its space and record the other in the free node 
		// 	       (B) the difference between them(small_suitable_size and the input size) is less than the free metadata
		// 	       so we will give the user all free spaces including the difference(because we can't put them at free node)
		// flag = 0 >> all free node has size smaller than the input size or there isn't free node (we must move ProgBrk)
		int flag = 0;
		
		// loop on the free node and test all cases
		while(temp != NULL)
		{
			// search on the free node which its size is equal to the input size "HAPPY CASE"
			if(temp->Blk_size == size)
			{
				// the "temp" pointer points to the free suitable node 
				// if this case happens
				// we will return the address of the "temp->next" to the user 
				retPtr = (void *)(&(temp->next));
				flag = 1;
#if(DEBUG == ON)
printf("Find THe Exact needed size\n");
printf("Pointer to the RetPtr = %p\n",retPtr);
printf("The Allocated size =  %ld \n", temp->Blk_size);
#endif

				// then we will start to delete this free node(because it becomes allocated)	
				// the linked list has only one node 
				// delete the unique node from the linkedlist
				if((temp->prev == NULL)&&(temp->next == NULL))
				{
					head = NULL;
					tail = NULL;
					break;
				}

				// delete the first node from the linkedlist
				if(temp->prev == NULL)
				{	
					// make the head points to the next node 
					head = temp->next;
					// make the the prev of the next node points to NULL
					temp->next->prev = NULL;
					break;
				}
				
				//delete the final node from the linkedlist
				if(temp->next == NULL)
				{
					// make the the next of the prev node points to NULL
					tail = temp->prev;
                                        temp->prev->next = NULL;
                                        break;

				}

				// Incase of the node locates at the middle of the linkedlist
				// create new node and it will points to previos node of the deleted node
				struct node *helpPtr = temp->prev;	
				helpPtr->next = temp->next;
				temp->next->prev = helpPtr;
				break;
			}
			else{
				// Incase of the free space is larger than the input size
				if(temp->Blk_size > size)
				{	
					// record the small suitable free size 
					// and put the addres of the suitable node at "ptr"
					if(temp->Blk_size < small_suitable_size)
					{
						small_suitable_size = temp->Blk_size;
						ptr = temp;
						flag = 2;
					}
					else{
						//nothing
					}
									
				}
				
			}
			temp = temp->next;
		}
		// Incase of the free space is larger than the input size
		if(flag == 2)
		{
			// we have two cases at this case
			// Case A : the difference between them(small_suitable_size and the input size) is grater than the free metadata
			// so we will give the user its space and record the other in the free node
			// we will called the node which user get it >> old node and the node which will have the diff size called new node
			// Don't forget >> we have a pointer to the suitable free node "ptr"
			if((ptr->Blk_size) - size >= FREE_METADATE)
			{
				// put the diff between sizes in this variable to create new node with this size
				size_t diff_size = (ptr->Blk_size) - size;

				// put at the current free node its new size
				ptr->Blk_size = size;
				// return to the user the address of the next ptr of the current free node
				retPtr = (void *)(&(ptr->next));
#if(DEBUG == ON)
printf("Find size and diff is greater \n");
printf("Pointer to the RetPtr = %p\n",retPtr);
printf("The Allocated size =  %ld \n", ptr->Blk_size);
#endif

				// move the "temp" pointer to the new free node space
				temp =(struct node *)((void *)ptr + size + ALLOC_METADATA); 
				// so the "ptr" points to the old free node {it becomes allocated}
				// and the "temp" points to the new free ndoe 
				
				// we want to record(create new node) the other size in the new free node
				// incase the old free node is the only node at the linkedlist
				if((ptr->prev == NULL)&&(ptr->next == NULL))
				{
					head = temp;
					tail = temp;
					temp->next = NULL;
					temp->prev = NULL;
					temp->Blk_size = diff_size -ALLOC_METADATA;
				}
				else{	
					//incase the old free node locates at the begaining of the linkedlist
					if(ptr->prev == NULL)
					{
						head = temp;
						temp->prev = NULL;
						ptr->next->prev = temp;
						temp->next = ptr->next;
						temp->Blk_size = diff_size - ALLOC_METADATA;
					}
					else{
						// incase the old free node locates at the end of the linkedlist
						if(ptr->next == NULL)
						{
							tail = temp;
							temp->next = NULL;
							ptr->prev->next = temp;
							temp->prev = ptr->prev;
							temp->Blk_size = diff_size - ALLOC_METADATA;
						}
						else{
							// incase the old free node locates at the center of the linkedlist
							ptr->prev->next = temp;
                               				ptr->next->prev = temp;
			                                temp->Blk_size = diff_size - ALLOC_METADATA; 
                        			        temp->next = ptr->next;
                                			temp->prev = ptr->prev;
						}

					}


				}

#if(DEBUG == ON)
printf("The remain size = %ld \n",temp->Blk_size);
printf("The Remain free node pointer = %p\n",temp);
printf("The Remain free node size = %ld\n",temp->Blk_size);
printf("The next of the new free node=  %p \n", temp->next);
printf("The prev of the new free node=  %p \n", temp->prev);
#endif

			}
			
			// Case B : the difference between them(small_suitable_size and the input size) is smaller than the free metadata
                        // so we will give the user all the free space at this free node then delete it from the linkedlist
			// DON't Forget >> we have a pointer to the suitable free node "ptr"
			else{

				// return the allocated pointer
				retPtr = (void *)(&(ptr->next));
                         	ptr->Blk_size = small_suitable_size;
#if(DEBUG == ON)
printf("Find size and diff is smaller \n");
printf("Pointer to the RetPtr = %p\n",retPtr);
printf("The Allocated size =  %ld \n", ptr->Blk_size);
#endif

				// the linked list has only one node 
				// delete the unique node from the linkedlist
				if((ptr->prev == NULL)&&(ptr->next == NULL))
				{
					head = NULL;
					tail = NULL;
				}
				else{	
					// delete the first node from the linkedlist
					if(ptr->prev == NULL)
					{	
						// make the head points to the next node 
						head = ptr->next;
						// make the the prev of the next node points to NULL
						ptr->next->prev = NULL;
					}
					else{
						//delete the final node from the linkedlist
						if(ptr->next == NULL)
						{
							// make the the next of the prev node points to NULL
							tail = ptr->prev;
                                        		ptr->prev->next = NULL;
						}
						else{
							 //incase the old free node locates at the center of the linkedlist
							 struct node *helpPtr = ptr->prev;
			                                 helpPtr->next = ptr->next;
                              				 ptr->next->prev = helpPtr;
						}
					}
				
				}
			}

		}
		else{
			// this means that there is no free space grater than or equal to the input size
			// so we must move the program break
			if(flag == 0)
			{
				 // check if the heap size is not suitable for the user to allocate
 		                 if((Prog_Brk + ((int)((size+ALL_METADATA)/PAGE_SIZE)+1)*PAGE_SIZE) >= HeapEnd)
                		 {
                        	 	 return NULL;
                  		}

				struct node *temp = Prog_Brk;

#if(DEBUG == ON)
printf("Can't Find size and we will move the program break\n");
printf("Prog_Brk(initially) = %p \n",Prog_Brk);
#endif
     		                // move the program break by the suitable page numbers
				 Prog_Brk = Prog_Brk + ((int)((size+ALL_METADATA)/PAGE_SIZE)+1)*PAGE_SIZE;
#if(DEBUG == ON)
printf("Suitable number of pages = %d\n",((int)((size+ALL_METADATA)/PAGE_SIZE)+1));
printf("Prog_Brk(finally) = %p \n",Prog_Brk);
#endif

                		// cast it to the size_varible and put at it the user need size
               		        *((size_t *)(temp)) = size;
		                // move this pointer to the next byte of the
                		// temp   --->|     size     | 8 byte
		                // retPtr --->|return pointer| void ptr
                		retPtr = (void *)((void *)temp + ALLOC_METADATA);
#if(DEBUG == ON)
printf("Ret Ptr = %p \n",retPtr);
printf("allocated size = %ld \n",*((size_t *)(temp)));
#endif

		                // move the temp to the next un used (will be free) bytes 
	        	        // then make the temp points to it then cast it to free node struct
                		//        --->|     size     | 8 byte
                		// retPtr --->|return pointer| void ptr
           		        //            |     ....     |
                		//            |     ....     |
		                // temp   --->|   free size  | >> #pages -input_size -2 * ALLOC_METADATA(8 byte)
                		//            |   next free  | >> NULL
                		//            |   prev free  | >> NULL
                		temp = (struct node *)(retPtr + size);
			

				// check if the linked list is empty
				// we will add the new free node at the first of the linkedlist
				if((head == NULL)&&(tail == NULL))
				{
					temp->Blk_size = ((int)((size+ALL_METADATA)/PAGE_SIZE)+1)*PAGE_SIZE - size - 2*ALLOC_METADATA;
					temp->next = NULL;
					temp->prev = NULL;
					head = temp;
					tail = temp; 
				}
				// if not we will add this node after the tail node
				else{
					temp->Blk_size = ((int)((size+ALL_METADATA)/PAGE_SIZE)+1)*PAGE_SIZE - size - 2*ALLOC_METADATA;
	        	        	temp->next = NULL;
					tail->next = temp;
        	        		temp->prev = tail;
					tail = temp;
				}
#if(DEBUG == ON)
printf("Pointer to the next free area = %p\n",temp);
printf("Next free size %ld \n", temp->Blk_size);
#endif
			}
			// flag = 1 
			// means that the input size is equal to certain free node so don't do anythig "I handle it at the above" 
			else{
				// do nothing
			}
	
		}
	}

return retPtr;
}


// Now the "Hmmfree" do three functions
// 1 - make the allocated block be free by adding this block at the free linkedlist
// 2 - merge the neigbours nodes 
// 3 - move the "Prog_Brk" if the free node locates at the end of the heap (it moves by multiple of the "PAGE_SIZE")


void *HmmFree(void *ptr)
{
	// check if the user enters "ptr" which is not at the region of the heap
	if((ptr < Heap_Start)||(ptr > HeapEnd))
	{
		return NULL;
	}

	// In this function , the user enters pointer and we want to use it to create new node at the address 
	// SO >> we have input : >>new free node << we will use this name in our function describtion
	struct node *ptr1 = head;
	struct node *ptr2 = head; 
	void *Retptr = ptr;
	int flag = 0;

	// initially we try to get the correct location of the new free node at the linkedlist
	while(ptr1 != NULL)
	{
		if((void *)ptr1 > ptr)
		{
			flag = 1;
			break;
		}
		ptr1 = ptr1->next;
	}
	// after the last while loop we have three cases
	// case 1 : the new free node locates before the head of the linkedlist
	// case 2 : the new free node locates at the middle of the free linkedlist 
	// case 3 : the new free node locates after the tail of the linkedlist
	// case 4 : The Free Linkedlist is empty so our new free node will be added to haed and tail
	
	// the flag = 1 if >> case 1 OR case 2
	if(flag == 1)
	{
		// handle case 1 
		if(ptr1 == head)
		{
			// Now we will use "ptr2" to points to the new free node 
			// and the "ptr1" points to the first node at the free linkedlist
			// we will add the new free node at the first of the linkedlist 
			// and make the head points to it
			ptr2 = (struct node *)(ptr - ALLOC_METADATA);
			ptr2->next =  ptr1;
			ptr2->prev = NULL;

			ptr1->prev = ptr2;
			head = ptr2;
#if(DEBUG == ON)
printf("Free Node locates before first free Node\n");

printf("The Free Size :  %ld \n", ((struct node *)ptr2)->Blk_size);
printf("The Free Next :  %p \n", ((struct node *)ptr2)->next);
printf("The Free Prev :  %p \n", ((struct node *)ptr2)->prev);
#endif


		}
		// handle case 2
		else {
			// Now we will use "ptr2" to points to the new free node 
			// and the "ptr1" points to the next node to the new free node 
			
			// make the next and prev of the new free node points to the next node and prev node 
                        ptr2 = (struct node *)(ptr - ALLOC_METADATA);
			ptr2->next = ptr1;
			ptr2->prev = ptr1->prev;

			// make the next of the last node refers to the new free node
			// make the prev of the next node refers to the new free node
			ptr1->prev->next = ptr2;
			ptr1->prev = ptr2;
#if(DEBUG == ON)
printf("Free Node locates at the middle of Linkedlist\n");
printf("The Free Size :  %ld \n", ((struct node *)ptr2)->Blk_size);
printf("The Free Next :  %p \n", ((struct node *)ptr2)->next);
printf("The Free Prev :  %p \n", ((struct node *)ptr2)->prev);
#endif
	
		}
	}
	else{
		// handle case 4
		if((head == NULL)&&(tail== NULL))
		{
			ptr2 = (struct node *)(ptr - ALLOC_METADATA);
			ptr2->next = NULL;
			ptr2->prev = NULL;
			tail = ptr2;
			head = ptr2;
#if(DEBUG == ON)
printf("Empty Free LinkedList \n");
printf("Free Node is alone at the free linkedlist \n");
printf("The Free Size :  %ld \n", ((struct node *)ptr2)->Blk_size);
printf("The Free Next :  %p \n", ((struct node *)ptr2)->next);
printf("The Free Prev :  %p \n", ((struct node *)ptr2)->prev);
#endif

		}
		else
		{
			// handle case 3 
			if((void *)tail < ptr)
			{
				// Now we will use "ptr2" to points to the new free node 
	                        // and the "tail" points to the last node at the free linkedlist
        	                // we will add the new free node after the tail node of the linkedlist 
                	        // and make the tail points to it
				ptr2 = (struct node *)(ptr - ALLOC_METADATA);
				ptr2->next =  NULL;
        	                ptr2->prev =  tail;

				tail->next = ptr2;
				tail = ptr2;
#if(DEBUG == ON)
printf("Free Node locates after last free Node\n");
printf("The Free Size :  %ld \n", ((struct node *)ptr2)->Blk_size);
printf("The Free Next :  %p \n", ((struct node *)ptr2)->next);
printf("The Free Prev :  %p \n", ((struct node *)ptr2)->prev);
#endif

			}	
		}
	}

	// we will call this function to merge the free nodes which are close 
	vdMergeFreeNodes();
	// then move the "Prog_Brk" if it can
	vdMoveBackwardPrgBrk();
	
	return (Retptr);

}


static void vdMergeFreeNodes()
{
	struct node * current = head;
	// the current node loop on the all free nodes (exept the tail node)
	 while((current != NULL)&&(current != tail))
        {
		// check if the current node address + current Blk Size equal to the next node
		// if happen we will remove the next node 
		// and increase the size of the of the current node
                if((((void *)(&current->next)) + current->Blk_size) == current->next)
		{
			struct node *help = current-> next->next;

			current->Blk_size = current->Blk_size + current->next->Blk_size + ALLOC_METADATA;
			current->next = help;
			if(help != NULL)
			help->prev = current;

			if(help == NULL)
				tail = current;
		}
        	current = current->next;
	}
}


static void vdMoveBackwardPrgBrk()
{
	// move the "Prog_Brk" if there is a free node at the end of the heap 
	// it moves by multiple of the "PAGE_SIZE"
	void *ptr =(void *)tail;
	if((ptr + ALLOC_METADATA + tail->Blk_size) == Prog_Brk)
	{
		int page_num    = (int)((tail->Blk_size )/PAGE_SIZE);
		int Remain_size = (int)((tail->Blk_size )%PAGE_SIZE);
		
		if(Remain_size>= POINTERS_FREENODE_SIZE)
		{
			tail->Blk_size = tail->Blk_size - page_num*PAGE_SIZE;
			Prog_Brk = Prog_Brk - page_num*PAGE_SIZE;
		}
		
	}


}


// this function are helper for the debug 
// you can display it by put "DEBUG" equal to "OFF"
#if(DEBUG == ON)
void vdDisplayFreeNode()
{
	 struct node *current = head;
	 int i =0;
         printf("Node #\t Size \t Address \t\t next \t Prev \n");

	 while((current != NULL))
        {
		printf("Node %d \t %ld \t %p \t %p \t %p \n",i,current->Blk_size,&(current->next),current->next,current->prev);
		i++;
		current = current->next;
	}
}
void vdDisplayPrgBrk()
{
	printf("Program Break : %p\n",Prog_Brk);
}


#endif
