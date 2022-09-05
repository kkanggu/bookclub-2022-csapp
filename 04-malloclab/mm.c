/*
 * mm-naive.c - The fastest, least memory-efficient malloc package.
 * 
 * In this naive approach, a block is allocated by simply incrementing
 * the brk pointer.  A block is pure payload. There are no headers or
 * footers.  Blocks are never coalesced or reused. Realloc is
 * implemented directly using mm_malloc and mm_free.
 *
 * NOTE TO STUDENTS: Replace this header comment with your own header
 * comment that gives a high level description of your solution.
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#include "mm.h"
#include "memlib.h"
#include "config.h"

/*********************************************************
 * NOTE TO STUDENTS: Before you do anything else, please
 * provide your team information in the following struct.
 ********************************************************/
team_t team = {
    /* Team name */
    "Soongsil-Developers",
    /* First member's full name */
    "kkanggu",
    /* First member's email address */
    "rica742244@gmail.com",
    /* Second member's full name (leave blank if none) */
    "",
    /* Second member's email address (leave blank if none) */
    ""
};

/*
 * | Padding1 | Prologue Header | Data Block | Epilogue Footer | Padding2
 * Pointer of allocated data points `Data Block`
 * Padding1 is 0 if not allocated, -1 if allocated
 * Padding2 is garbage value
 * Prologue Header and Epilogue Footer is `Data SIZE | 000`, if allocated, this |= 0x1
 */

static void * mm_head_ptr ;         // Point data block of first block
static void * mm_tail_ptr ;         // Point data block of last block
static void * mm_current_ptr ;      // Point last allocated data block
static void * mm_MAX_HEAP ;         // Point last memory of heap


/* 
 * mm_init - initialize the malloc package.
 * 
 * to use my malloc package, you must call this function first.
 *
 * ret
 * -1   initialization failed.
 * 0    initialization succeeded.
 *
 */
int mm_init(void)
{
    void * ptr = mem_sbrk ( MALLOC_SIZE + WSIZE * 4 ) ;             // Malloc 1 << 12 at initial
    void * setPtr ;



    if ( ( void * ) -1 == ptr )
    {
        return -1 ;
    }

    memset ( ptr , 0 , MALLOC_SIZE + WSIZE * 4 ) ;
    ptr += WSIZE << 1 ;
    setPtr = GET_PROLOGUE_HEADER ( ptr ) ;
    * ( size_t * ) setPtr = PACK ( MALLOC_SIZE , 0 ) ;
    setPtr = GET_EPILOGUE_FOOTER ( ptr ) ;
    * ( size_t * ) setPtr = PACK ( MALLOC_SIZE , 0 ) ;

    mm_head_ptr = ptr ;
    mm_tail_ptr = ptr ;
    mm_current_ptr = ptr ;
    mm_MAX_HEAP = ptr - WSIZE * 2 + MAX_HEAP ;

    return 0 ;
}

void * mm_coalesce_block ( void * ptr )
{
    void * tempPtr ;
    void * setPtr ;



    if ( ptr != mm_head_ptr )   // Check former block
    {
        tempPtr = GET_PREV_BLOCK ( ptr ) ;

        if ( 0 == GET_ALLOC ( tempPtr ) )
        {
            size_t size = GET_SIZE ( tempPtr ) + GET_SIZE ( ptr ) + WSIZE * 4 ;


            setPtr = GET_EPILOGUE_FOOTER ( tempPtr ) ;
            * ( size_t * ) setPtr = 0 ;
            setPtr = GET_PROLOGUE_HEADER ( ptr ) ;
            * ( size_t * ) setPtr = 0 ;

            setPtr = GET_PROLOGUE_HEADER ( tempPtr ) ;
            * ( size_t * ) setPtr = PACK ( size , 0 ) ;
            setPtr = GET_EPILOGUE_FOOTER ( tempPtr ) ;
            * ( size_t * ) setPtr = PACK ( size , 0 ) ;


            ptr = tempPtr ;
        }
    }
    else if ( ptr != mm_tail_ptr )      // Check latter block
    {
        tempPtr = GET_NEXT_BLOCK ( ptr ) ;

        if ( 0 == GET_ALLOC ( tempPtr ) )
        {
            size_t size = GET_SIZE ( ptr ) + GET_SIZE ( tempPtr ) + WSIZE * 4 ;


            setPtr = GET_EPILOGUE_FOOTER ( ptr ) ;
            * ( size_t * ) setPtr = 0 ;
            setPtr = GET_PROLOGUE_HEADER ( tempPtr ) ;
            * ( size_t * ) setPtr = 0 ;

            setPtr = GET_PROLOGUE_HEADER ( ptr ) ;
            * ( size_t * ) setPtr = PACK ( size , 0 ) ;
            setPtr = GET_EPILOGUE_FOOTER ( ptr ) ;
            * ( size_t * ) setPtr = PACK ( size , 0 ) ;
        }
    }


    return ptr ;
}

int mm_extend_heap ( void )
{
    void * newPtr = mem_sbrk ( MALLOC_SIZE + WSIZE * 4 ) ;
    void * setPtr ;



    if ( ( void * ) -1 == newPtr )
    {
        return -1 ;
    }

    memset ( newPtr , 0 , MALLOC_SIZE + WSIZE * 4 ) ;
    newPtr += WSIZE << 1 ;
    setPtr = GET_PROLOGUE_HEADER ( newPtr ) ;
    * ( size_t * ) setPtr = PACK ( MALLOC_SIZE , 0 ) ;
    setPtr = GET_EPILOGUE_FOOTER ( newPtr ) ;
    * ( size_t * ) setPtr = PACK ( MALLOC_SIZE , 0 ) ;

    newPtr = mm_coalesce_block ( newPtr ) ;

    mm_tail_ptr = newPtr ;
    mm_current_ptr = mm_tail_ptr ;


    return 0 ;
}

/*
 * Find unallocated data block can contain iSize data
 * Find until meet mm_tail_ptr, then start from mm_head_ptr
 * If can't find unallocated data block, extend heap until can contain iSize and return unallocated block that generated
 */
void * mm_find_next_fit ( int iSize )
{
    void * nextPtr ;



    for ( nextPtr = mm_current_ptr ; nextPtr <= mm_tail_ptr ; nextPtr = GET_NEXT_BLOCK ( nextPtr ) )
    {
        if ( ( 0 == GET_ALLOC ( nextPtr ) ) && ( iSize <= GET_SIZE ( nextPtr ) ) )
            return nextPtr ;
    }
    for ( nextPtr = mm_head_ptr ; nextPtr < mm_current_ptr ; nextPtr = GET_NEXT_BLOCK ( nextPtr ) )
    {
        if ( ( 0 == GET_ALLOC ( nextPtr ) ) && ( iSize <= GET_SIZE ( nextPtr ) ) )
            return nextPtr ;
    }


    if ( -1 == mm_extend_heap () )
        return -1 ;

    while ( GET_SIZE ( mm_tail_ptr ) < iSize )      // Extend heap until can contain iSize
    {
        if ( -1 == mm_extend_heap () )
            return -1 ;
    }


    return mm_tail_ptr ;
}

void mm_separate_block ( void * ptr , size_t size )
{
    size_t rightSize = GET_SIZE ( ptr ) - size - 16 ;
    void * setPtr ;



    setPtr = GET_PROLOGUE_HEADER ( ptr ) ;
    * ( size_t * ) setPtr = PACK ( size , 0 ) ;
    setPtr = GET_EPILOGUE_FOOTER ( ptr ) ;
    * ( size_t * ) setPtr = PACK ( size , 0 ) ;


    ptr = GET_NEXT_BLOCK ( ptr ) ;
    setPtr = GET_PROLOGUE_HEADER ( ptr ) ;
    * ( size_t * ) setPtr = PACK ( rightSize , 0 ) ;
    setPtr = GET_EPILOGUE_FOOTER ( ptr ) ;
    * ( size_t * ) setPtr = PACK ( rightSize , 0 ) ;

    ptr = GET_PREV_BLOCK ( ptr ) ;

    if ( ptr == mm_tail_ptr )
        mm_tail_ptr = GET_NEXT_BLOCK ( ptr ) ;
}

/* 
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *
 * Always allocate a block whose size is a multiple of the alignment.
 * size of block is AT LEAST size(param) byte. it would be bigger than
 * size(param)
 *
 *ret
 * NULL failed.
 * ptr  valid address of allocated block PAYLOAD.
 */
void* mm_malloc(size_t size)
{
    if ( 0 == size )
    {
        return NULL ;
    }

    int iDataSize = ALIGN ( size ) ;
    void * ptr = mm_find_next_fit ( iDataSize ) ;
    void * setPtr ;



    if ( ( void * ) -1 == ptr )         // Run out of memory
    {
        return NULL ;
    }

    if ( 24 <= GET_SIZE ( ptr ) - size )       // Can separate at least 8bytes
    {
        mm_separate_block ( ptr , iDataSize ) ;
    }
    else
    {
        iDataSize = GET_SIZE ( ptr ) ;
    }


    memset ( GET_PADDING ( ptr ) , -1 , WSIZE ) ;                       // Fill padding with 11....
    memset ( GET_PROLOGUE_HEADER ( ptr ) , 0 , iDataSize + WSIZE * 3 ) ;

    setPtr = GET_PROLOGUE_HEADER ( ptr ) ;
    * ( size_t * ) setPtr = PACK ( iDataSize , 1 ) ;
    setPtr = GET_EPILOGUE_FOOTER ( ptr ) ;
    * ( size_t * ) setPtr = PACK ( iDataSize , 1 ) ;


    if ( ptr != mm_tail_ptr )
    {
        setPtr = GET_NEXT_BLOCK ( ptr ) ;
        mm_coalesce_block ( setPtr ) ;
    }


    return ptr ;
}

/*
 * mm_free - Freeing a block.
 */
void mm_free(void *ptr)
{
    size_t size = GET_SIZE ( ptr ) ;
    void * setPtr ;
    
    

    setPtr = GET_PROLOGUE_HEADER ( ptr ) ;
    * ( size_t * ) setPtr = PACK ( size , 0 ) ;
    setPtr = GET_EPILOGUE_FOOTER ( ptr ) ;
    * ( size_t * ) setPtr = PACK ( size , 0 ) ;

    setPtr = mm_coalesce_block ( ptr ) ;

    if ( setPtr != ptr )        // coalesced, change mm_tail_ptr and mm_current_ptr
    {
        if ( mm_current_ptr == ptr )
            mm_current_ptr = setPtr ;
        if ( mm_tail_ptr == ptr )
            mm_tail_ptr = setPtr ;
    }
}

/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 *
 * (not now)
 */
void *mm_realloc(void *ptr, size_t size)
{
    void * newPtr = mm_malloc ( size ) ;

    if ( NULL == newPtr )       // Run out of memory
        return NULL ;

    size_t copyDataSize = GET_SIZE ( ptr ) ;

    if ( copyDataSize > size )
        copyDataSize = size ;

    memcpy ( newPtr , ptr , copyDataSize ) ;

    mm_free ( ptr ) ;


    return newPtr ;
}

//----------mine----------

/*
 * error checked mm_init. 
 */
void Mm_init(void)
{
    if(mm_init() == -1){      
        puts("mm_init failed!");
        exit(1);
    }
}