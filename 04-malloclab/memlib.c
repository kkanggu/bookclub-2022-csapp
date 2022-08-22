/*
 * memlib.c - a module that simulates the memory system.  Needed because it 
 *            allows us to interleave calls from the student's malloc package 
 *            with the system's malloc package in libc.
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>
#include <errno.h>

#include "memlib.h"
#include "config.h"

/* private variables */
static char *mem_start_brk;  /* points to first byte of heap */
static char *mem_brk;        /* points to last byte of heap */
static char *mem_max_addr;   /* largest legal heap address */ 

/* 
 * mem_init - initialize the memory system model
 */
void mem_init(void)
{
    /* allocate the storage we will use to model the available VM */
    if ((mem_start_brk = (char *)malloc(MAX_HEAP)) == NULL) {
        fprintf(stderr, "mem_init_vm: malloc error\n");
        exit(1);
    }

    mem_max_addr = mem_start_brk + MAX_HEAP;  /* max legal heap address */
    mem_brk = mem_start_brk;                  /* heap is empty initially */
}

/* 
 * mem_deinit - free the storage used by the memory system model
 */
void mem_deinit(void)
{
    free(mem_start_brk);
}

/*
 * mem_reset_brk - reset the simulated brk pointer to make an empty heap
 */
void mem_reset_brk()
{
    mem_brk = mem_start_brk;
}

/* 
 * mem_sbrk - simple model of the sbrk function. Extends the heap 
 *    by incr bytes and returns the start address of the new area. In
 *    this model, the heap cannot be shrunk.
 */
void *mem_sbrk(int incr) 
{
    char *old_brk = mem_brk;

    if ( incr < 0 )
    {
        errno = ENOMEM;
        fprintf(stderr, "ERROR: mem_sbrk failed. Request minus size\n");
        return (void *)-1;
    }
    else if ( mem_max_addr < ( mem_brk + incr ) )
    {
	    if ( NULL == ( old_brk = kg_mem_find_empty_space ( incr ) ) )       // Find empty place from mem_start_brk
        {
            kg_coalesce_heap () ;                       // If NULL, coalesce heap to reduce external fragmentation

	        if ( NULL == ( old_brk = kg_mem_find_empty_space ( incr ) ) )   // If NULL, too less memory.
            {
                errno = ENOMEM;
                fprintf(stderr, "ERROR: mem_sbrk failed. Ran out of memory\n");
                return (void *)-1;
            }
        }

        mem_brk = old_brk ;
    }
    mem_brk += incr;
    return (void *)old_brk;
}
    
/*
 * mem_heap_lo - return address of the first heap byte
 */
void *mem_heap_lo()
{
    return (void *)mem_start_brk;
}

/* 
 * mem_heap_hi - return address of last heap byte
 */
void *mem_heap_hi()
{
    return (void *)(mem_brk - 1);
}

/*
 * mem_heapsize() - returns the heap size in bytes
 */
size_t mem_heapsize() 
{
    return (size_t)(mem_brk - mem_start_brk);
}

/*
 * mem_pagesize() - returns the page size of the system
 */
size_t mem_pagesize()
{
    return (size_t)getpagesize();
}

void kg_coalesce_heap ( void )
{
    char * left_empty_ptr = mem_start_brk + WSIZE ;
    char * right_data_ptr = mem_start_brk + WSIZE ;
    int iSize ;



    while ( left_empty_ptr <= mem_max_addr )
    {
        if ( 0 == * ( int * ) left_empty_ptr )       // Find empty space, move right_data_ptr to left_empty_ptr
        {
            while ( right_data_ptr <= mem_max_addr )
            {
                if ( 0 != * ( int * ) right_data_ptr )
                {
                    iSize = GET_SIZE ( right_data_ptr + WSIZE * 3 ) + ( WSIZE << 2 ) ;

                    memcpy ( left_empty_ptr , right_data_ptr , iSize ) ;

                    break ;
                }
                else
                    right_data_ptr = right_data_ptr + DSIZE ;
            }

            if ( right_data_ptr > mem_max_addr )
                break ;
        }
        else
            left_empty_ptr = left_empty_ptr + DSIZE ;
    }
}

void* kg_mem_find_empty_space ( int iSize )
{
    char * targetPtr = mem_start_brk + WSIZE ;
    char * tempPtr ;
    int iTempSize = 0 ;



    while ( targetPtr + iSize <= mem_max_addr )
    {
        iTempSize = 0 ;


        if ( -1 == * ( int * ) targetPtr )      // Find Padding
        {
            targetPtr = targetPtr + WSIZE * 3 ;
            targetPtr = GET_EPILOGUE_FOOTER ( targetPtr ) + WSIZE ;
        }
        else                                    // Find empty memory
        {
            tempPtr = targetPtr ;

            while ( iTempSize != iSize )
            {

                if ( 0 != * ( int * ) tempPtr )           // If find allocated memory, can't allocated at this memory
                    break ;

                tempPtr = tempPtr + DSIZE ;
                iTempSize += DSIZE ;
            }

            if ( iTempSize == iSize )
                return targetPtr ;
        }
    }


    return NULL ;           // Can't find empty memory
}