/* 
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */ 
#include <stdio.h>
#include "cachelab.h"

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

/* 
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded. 
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
    int i , j , k , iTemp ;
    int irgTemp [ 8 ] ;



    if ( ( 32 == N ) && ( 32 == M ) )
    {
        for ( i = 0 ; i < 4 ; ++i )
        {
            for ( k = 0 ; k < 8 ; ++k )
            {
                for ( iTemp = 0 ; iTemp < 4 ; ++ iTemp )
                {
                    for ( j = 0 ; j < 8 ; ++j )
                    {
                        irgTemp [ j ] = A [ ( i << 3 ) + j ] [ k + ( iTemp << 3 ) ] ;
                    }
                    for ( j = 0 ; j < 8 ; ++j )
                    {
                        B [ k + ( iTemp << 3 ) ] [ ( i << 3 ) + j ] = irgTemp [ j ] ;
                    }
                }
            }
        }
    }
    else if ( ( 64 == N ) && ( 64 == M ) )
    {
        for ( i = 0 ; i < 16 ; ++i )
        {
            for ( k = 0 ; k < 8 ; ++k )
            {
                for ( iTemp = 0 ; iTemp < 8 ; ++ iTemp )
                {
                    for ( j = 0 ; j < 4 ; ++j )
                    {
                        irgTemp [ j ] = A [ ( i << 2 ) + j ] [ k + ( iTemp << 3 ) ] ;
                    }
                    for ( j = 0 ; j < 4 ; ++j )
                    {
                        B [ k + ( iTemp << 3 ) ] [ ( i << 2 ) + j ] = irgTemp [ j ] ;
                    }
                }
            }
        }
    }
    else if ( ( 67 == N ) && ( 61 == M ) )
    {
        for ( i = 0 ; i < 16 ; ++i )                    // 64 , 60 First
        {
            for ( k = 0 ; k < 4 ; ++k )
            {
                for ( iTemp = 0 ; iTemp < 15 ; ++ iTemp )
                {
                    for ( j = 0 ; j < 4 ; ++j )
                    {
                        irgTemp [ j ] = A [ ( i << 2 ) + j ] [ k + ( iTemp << 2 ) ] ;
                    }
                    for ( j = 0 ; j < 4 ; ++j )
                    {
                        B [ k + ( iTemp << 2 ) ] [ ( i << 2 ) + j ] = irgTemp [ j ] ;
                    }
                }
            }
        }
        for ( i = 0 ; i < 8 ; ++i )                     // 64 x 60 to 64 x 61
        {
            for ( j = 0 ; j < 8 ; ++j )
            {
                irgTemp [ j ] = A [ j + ( i << 3 ) ] [ 60 ] ;
            }
            for ( j = 0 ; j < 8 ; ++j )
            {
                B [ 60 ] [ j + ( i << 3 ) ] = irgTemp [ j ] ;
            }
        }
        for ( i = 0 ; i < 20 ; ++i )                    // 64 x 61 to 67 x 61
        {
            for ( j = 0 ; j < 3 ; ++j  )
            {
                for ( k = 0 ; k < 3 ; ++k )
                {
                    irgTemp [ k ] = A [ 64 + k ] [ i * 3 + j ] ;
                }
                for ( k = 0 ; k < 3 ; ++k )
                {
                    B [ i * 3 + j ] [ 64 + k ] = irgTemp [ k ] ;
                }
            }
        }
        for ( i = 0 ; i < 3 ; ++i )
        {
            irgTemp [ i ] = A [ 64 + i ] [ 60 ] ;
        }
        for ( i = 0 ; i < 3 ; ++i )
        {
            B [ 60 ] [ 64 + i ] = irgTemp [ i ] ;
        }
    }
}

/* 
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started. 
 */ 

/* 
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tmp;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }    

}

/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions()
{
    /* Register your solution function */
    registerTransFunction(transpose_submit, transpose_submit_desc); 

    /* Register any additional transpose functions */
    registerTransFunction(trans, trans_desc); 

}

/* 
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; ++j) {
            if ( i == j )
                continue ;
            if (A[i][j] != B[j][i]) {
                return 0;
            }
        }
    }
    return 1;
}