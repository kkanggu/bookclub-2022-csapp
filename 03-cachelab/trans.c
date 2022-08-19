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
typedef void (*per_block_t)(int M, int N, int input[N][M], int output[M][N], int row, int col);
void square(int M, int N, int input[N][M], int output[M][N], int row, int col);
void square_hard(int M, int N, int input[N][M], int output[M][N], int row, int col);
void naive(int M, int N, int input[N][M], int output[M][N], int row, int col);

void transpose(int M, int N, int input[N][M], int output[M][N])
{
  per_block_t per_block;
  if (M == 32 && N == 32) {
    per_block = square;
  } else if (M == 64 && N == 64) {
    per_block = square_hard;
  } else {
    per_block = naive;
  }

  for (int row = 0; row < N; row += 8) {
    for (int col = 0; col < M; col += 8) {
      per_block(M, N, input, output, row, col);
    }
  }
}

void square(int M, int N, int input[N][M], int output[M][N], int row, int col) {
  if (row != col) { return naive(M, N, input, output, row, col); }

  // Special handling for same block
  for (int i = row; i < row + 8 && i < N; ++i) {
    int tmp = input[i][i];
    for (int j = col; j < col + 8 && j < M; ++j) {
      // "output[j][i] = input[i][j]" for same i, j will cause cache miss.
      if (i == j) { continue; }
      output[j][i] = input[i][j];
    }
    output[i][i] = tmp;
  }
}

void square_hard(int M, int N, int input[N][M], int output[M][N], int row, int col) {
  // 첫번째줄 미리 캐싱
  int *t = &input[col][row + 4];
  int a = t[0], b = t[1], c = t[2], d = t[3];

  // output의 윗쪽 반
  for (int k = 0; k < 8; ++k) {
    int *t = &input[col + k][row];
    int a = t[0], b = t[1], c = t[2], d = t[3];
    t = &output[row][col + k];
    t[0] = a; t[64] = b; t[128] = c; t[192] = d;
  }

  // output의 아랫쪽 반
  for (int k = 7; k > 0; --k) {
    int *t = &input[col + k][row + 4];
    int a = t[0], b = t[1], c = t[2], d = t[3];
    t = &output[row + 4][col + k];
    t[0] = a; t[64] = b; t[128] = c; t[192] = d;
  }

  // 캐싱된 첫번째줄 사용
  t = &output[row + 4][col];
  t[0] = a; t[64] = b; t[128] = c; t[192] = d;
}

void naive(int M, int N, int input[N][M], int output[M][N], int row, int col) {
  for (int i = col; i < col + 8 && i < M; ++i) {
    for (int j = row; j < row + 8 && j < N; ++j) {
      output[i][j] = input[j][i];
    }
  }
}

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
    registerTransFunction(transpose, "Transpose submission"); 

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
            if (A[i][j] != B[j][i]) {
                return 0;
            }
        }
    }
    return 1;
}

