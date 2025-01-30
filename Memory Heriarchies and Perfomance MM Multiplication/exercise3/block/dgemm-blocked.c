#include <immintrin.h> // For AVX as we saw in the class slide HPC_2024_L1 and the provided example code with the slides 
#include <math.h>  // for calculating the square root

const char* dgemm_desc = "Blocked dgemm.";

/* This routine performs a dgemm operation
 *
 *  C := C + A * B
 *
 * where A, B, and C are lda-by-lda matrices stored in column-major format. 
 * 
 * This means that elements in the same column are stored next to each other in memory like: 
 * [a11, a21, a31, a12, a22, a32, a13, a23, a33] in a 3x3 case. -> https://en.wikipedia.org/wiki/Row-_and_column-major_order
 * THEN I SHOULD INDEX BY COLUMN AND THEN BY ROW!
 * 
 * On exit, A and B maintain their input values.
 * 
 */

//First, two create the blocks we should take into account the size of the L3 cache, (i will only implement it as fitting it into L3, not optimized for L2 and L1)
#define L3_CACHE_SIZE 26214400  // since we studied in the second exercise that the L3 cache size is 25MB, i will use this value to determine the block size
// https://www.gbmb.org/mb-to-bytes 

//now to define the block size we need to account for the fact the we use doubles -> 8 bytes. and that there are 3 matrices:
// (Block size)^2 = (L3_CACHE_SIZE / (3*8)) from the whole cache size we need to extract the amount of "units" that can store doubles (8 bytes) 
//but also, divide that into 3 matrices. but since is a square matrix, we can just take the square root of the result to get the block size
//Block size = (L3_CACHE_SIZE / (3*8))^(1/2), which should return an int 
#define BLOCK_SIZE ((int)sqrt(L3_CACHE_SIZE / (3 * 8)))
//i tried first using floor function but turns out that return double, therefore, for the indexing i need to 
// "casting" to int -> https://cplusplus.com/forum/beginner/47671/

int min(int a, int b) {
    if (a > b){
        return b;
    }
    return a;
}

void matrix_multiply_block(int n,int I, int J, int K, double* A, double* B, double* C)
{
    for (int j = 0; j < J; ++j) {
        for (int k = 0; k < K; ++k) {
            //since we are using column-major format we need to jump to the right column j*n and then move i steps into  the column meaning index of row
            //example : a22 would be column 1 * n + 1 with mean in a 3x3 matrix as example at the beginning, 1*3 = 3 + 1 of the row  = 4 
            for (int i = 0; i < I; ++i) {
                /*"If an inner loop variable is used as an index to a multidimensional array, it should be the index that ensures stride-one access"
                    p. 70 book
                    Therefore, following book instructions i access the C and A as consecutive elements in column -> stride one access
                    while keeping b constant - same element - repeatedly. 
                    */
                C[i + j*n] += A[i + k*n] * B[k + j*n];
            }
        }
    }
}


void square_dgemm(int n, double* A, double* B, double* C) {
  int block_size = min(BLOCK_SIZE, n); //since the block size should be smaller than the matrix size

  //following the blocked matrix multiply algorithm teached in the last class and ch 3 book 
  for (int j = 0; j < n; j += block_size) {
      for (int k = 0; k < n; k += block_size) {
          for (int i = 0; i < n; i += block_size) {
            int I = min(block_size, n-i);
            int J = min(block_size, n-j);
            int K = min(block_size, n-k);
            /* 
            since we pass a pointer with the adress of the beginning of the matrix (double* A - as we saw in previous assignment) 
            then, we need to move in each block iteration the pointer to the right position of the block. 
            meaning, we take the first block A + 0 + 0 meaning, initial block then inside matrix_multiply we will cover the block_size length only
            therefore, on A + 0 + 1 * n, we will start in the next row - in this case since is A -  of the block, and so on.
            */
            matrix_multiply_block(n, I, J, K, A + i + k*n, B + k + j*n, C + i + j*n);
          }
      }
  }
}
