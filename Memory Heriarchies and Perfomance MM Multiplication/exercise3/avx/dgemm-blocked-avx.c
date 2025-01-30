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

//now to use AVX as we saw in the tutorial, we saw that we can only load 4 doubles at a time.
//since each double is 8 bytes - 64 bits, then 256 the total 4 doubles.

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
            __m256d b0 = _mm256_broadcast_sd(&B[k + j*n]); //we broadcast the b repeated elements that we need to use -> since until k or j moves, we use the same value under i for each operation -> see illustration
            int i; //the idea was to create i outside, so, in the case i overpass matrix limit, i continue with the "normal way" to not have indexing problem if its not divisible by 4.
            for (i = 0; i <= I - 4; i += 4) {
                //we load the four possible doubles
                __m256d c0 = _mm256_loadu_pd(&C[i + j*n]);
                __m256d a0 = _mm256_loadu_pd(&A[i + k*n]);
                //here we do the matrix multiplication as i remember with fma explanation of intel:
                // "Dot product, matrix multiplication and polynomial evaluations are expected to benefit from the use of FMA, 256 - 
                //bit data path and the independent executions on two ports. The peak throughput of FMA from each processor core are 16 single-precision and 8 double-precision results each cycle"
                c0 = _mm256_fmadd_pd(a0, b0, c0); // the idea came from eth slide attached on the footnote, however, i needed to check this: https://portal.nacad.ufrj.br/online/intel/compiler_c/common/core/GUID-2F190FEC-7C2F-44A6-B81E-F6026DDA1F45.htm
                //to actually be sure which are the inputs.
                _mm256_storeu_pd(&C[i + j*n], c0); //as on class example.
            }
            for (i; i < I; ++i) {
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