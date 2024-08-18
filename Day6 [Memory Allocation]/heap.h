// you can change the "HEAP_SIZE" by this macro
// you can assign the value of the "PAGE_SIZE"
// and it will be used to define the "Program break" move
// so the program break moves by number which is multiple of the "PAGE_SIZE"
// for ex >> one page size or two and so on
#define HEAP_SIZE 100000000
#define PAGE_SIZE 100

// the size of the metadata of the allocated Block
#define ALLOC_METADATA		(sizeof(size_t))
// the size of the pointers which needed for the free node
#define POINTERS_FREENODE_SIZE	(2*sizeof(void *))
// the size of the metadata of the free Block
#define FREE_METADATE		((sizeof(size_t)) + (2*sizeof(void *)))
// the summation of the FREE_METADATE and ALLOC_METADATA
#define ALL_METADATA		(ALLOC_METADATA + FREE_METADATE)


// this macros are used to enable the debug mode
#define ON  1
#define OFF 0
#define DEBUG	ON

// this type will allocate the size of the allocated block or free
typedef long unsigned int size_t;

void *HmmAlloc(size_t size);
void *HmmFree(void *ptr);
static void vdMoveBackwardPrgBrk();
static void vdMergeFreeNodes();

// this functions are used for the debugging
#if(DEBUG == ON)
void vdDisplayFreeNode();
void vdDisplayPrgBrk();
#endif



