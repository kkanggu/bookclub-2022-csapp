#include <stdio.h>
#include <assert.h>

extern int      mm_init(void);
extern void*    mm_malloc(size_t size);
extern void     mm_free(void* ptr);
extern void*    mm_realloc(void* ptr, size_t size);
//mine
extern void     Mm_init(void);
#define WSIZE       4
#define DSIZE       8

/* single word (4) or double word (8) alignment */
#define ALIGNMENT   8

/* Pack a size and allocated bit int a word */
#define PACK(size, alloc)   ((size) | (alloc))

/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size)     (((size) + (ALIGNMENT-1)) & ~0x7)

/* Read and write a word at address p */
#define GET(p)          (*(unsigned int*)(p)) 
#define PUT(p, val)     (*(unsigned int*)(p) = (val))

/* Get Prologue Header ptr bp, get Padding, Prologue Footer, Data block, Epilogue Footer */
#define GET_PADDING(bp)                 ( ( char * ) bp - ( WSIZE << 1 ) )
#define GET_PROLOGUE_HEADER(bp)         ( ( char * ) bp - WSIZE )
#define GET_EPILOGUE_FOOTER(bp)         ( ( char * ) bp + GET_SIZE ( bp ) )

#define GET_PREV_BLOCK(bp)              ( ( char * ) bp - ( * ( unsigned int * ) ( bp - ( WSIZE << 2 ) ) & ~0x7 ) - ( WSIZE << 2 ) )
#define GET_NEXT_BLOCK(bp)              ( ( char * ) bp + GET_SIZE ( bp ) + ( WSIZE << 2 ) )

/* Read the size and allocated fields from Prologue Header */
#define GET_SIZE(p)     ( GET ( GET_PROLOGUE_HEADER ( p ) ) & ~0x7 )
#define GET_ALLOC(p)    ( GET ( GET_PROLOGUE_HEADER ( p ) ) & 0x1 )

#define SIZE_T_SIZE     (ALIGN(sizeof(size_t)))

#define MALLOC_SIZE     ( 1 << 12 )

/* 
 * Students work in teams of one or two.  Teams enter their team name, 
 * personal names and login IDs in a struct of this
 * type in their bits.c file.
 */
typedef struct {
    char*   teamname; /* ID1+ID2 or ID1 */
    char*   name1;    /* full name of first member */
    char*   id1;      /* login ID of first member */
    char*   name2;    /* full name of second member (if any) */
    char*   id2;      /* login ID of second member */
} team_t;

extern team_t team;
   
