#include <unistd.h>

#define WSIZE       4
#define DSIZE       8
#define GET_SIZE(p)    				( ( * ( unsigned int * ) ( p ) ) & ~0x7 )
#define GET_PADDING(bp)				( ( char * ) bp - 3 * WSIZE )
#define GET_PROLOGUE_HEADER(bp)     ( ( char * ) bp - ( WSIZE << 1 ) )
#define GET_PROLOGUE_FOOTER(bp)     ( ( char * ) bp - WSIZE )
#define GET_EPILOGUE_FOOTER(bp)     ( ( char * ) bp + GET_SIZE ( bp ) )

void mem_init(void);               
void mem_deinit(void);
void* mem_sbrk(int incr);
void mem_reset_brk(void); 
void* mem_heap_lo(void);
void* mem_heap_hi(void);
size_t mem_heapsize(void);
size_t mem_pagesize(void);

void kg_coalesce_heap ( void ) ;
void * kg_mem_find_empty_space ( int iSize ) ;