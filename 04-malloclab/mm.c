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
    "rica742244@gmail.com"
    /* Second member's full name (leave blank if none) */
    "",
    /* Second member's email address (leave blank if none) */
    ""
};

/*
 * | Padding | Prologue Header | Prologue Footer | Data | Epilogue Footer
 * Pointer of allocated data points `Prologue Header`
 * Prologue Header is `Data SIZE | 000`, if allocated, this |= 0x1
 * Prologue Footer and Epilogue Footer is 0, if allocated, this = 1
 */


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
    if(mem_sbrk(WSIZE) == (void*)-1){
        return -1;
    }
    return 0;
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
    void * ptr = mem_sbrk ( iDataSize + 4 * WSIZE ) ;
    void * setPtr ;

    if ( ( void * ) -1 == ptr )         // Run out of memory
        return NULL ;

    memset ( ptr , -1 , WSIZE ) ;        // Fill padding with 11....
    memset ( ptr + WSIZE , 0 , iDataSize + 3 * WSIZE ) ;
    ptr = ( char * ) ptr + WSIZE ;    
    
    ptr = PACK ( iDataSize , 1 ) ;       // Set Prologue Header
    setPtr = ( char * ) ptr + WSIZE ;
    setPtr = PACK ( 0 , 1 ) ;       // Set Prologue Footer
    setPtr = ( char * ) ptr + iDataSize + WSIZE ;
    setPtr = PACK ( 0 , 1 ) ;       // Set Epilogue Footer


    return ptr ;
}

/*
 * mm_free - Freeing a block.
 */
void mm_free(void *ptr)
{
    int iSize = GET_SIZE_BITS ( * ( ( int * ) ptr ) ) ;
    
}

/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 *
 * (not now)
 */
void *mm_realloc(void *ptr, size_t size)
{
    void *oldptr = ptr;
    void *newptr;
    size_t copySize;
    
    newptr = mm_malloc(size);
    if (newptr == NULL)
      return NULL;
    copySize = *(size_t *)((char *)oldptr - SIZE_T_SIZE);
    if (size < copySize)
      copySize = size;
    memcpy(newptr, oldptr, copySize);
    mm_free(oldptr);
    return newptr;
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