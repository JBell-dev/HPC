#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "walltime.h"
#include <omp.h>

int main(int argc, char *argv[]) {
    int N = 2000000000;
    double up = 1.00000001;
    double Sn = 1.00000001;
    int n;

    double *opt = (double *)malloc((N + 1) * sizeof(double));
    if (opt == NULL) {
        perror("failed to allocate problem size");
        exit(EXIT_FAILURE);
    }

    double time_start = walltime();
    
    // we get the chunks given the number of available threads
    int max_threads = omp_get_max_threads();
    int thread_chunk = (N + 1) / max_threads; //this is how much each thread will get given static schedulling
    //since if no chunk size is specified, omp divides the total number of iterations equally among the threads.

    // i took advantage of cache memory as we saw on previous assignment. 
    const int CHUNK_SIZE = 1024;
    double *chunk = (double *)malloc(CHUNK_SIZE * sizeof(double));
    
    // then i fill it up based on our "up" update : chunk[0] = 1.0, chunk[1] = up, chunk[2] = up^2, so on....
    chunk[0] = 1.0;
    for(int i = 1; i < CHUNK_SIZE; i++) {
        chunk[i] = chunk[i-1] * up;
    }
    
    // this will be our final updater such that it leave us on the start value of the next "chunk" of calculation
    //this would help us to update our "sn" as it correspond to the beginning fo the chunk we are calculating on 
    double chunk_multiplier = chunk[CHUNK_SIZE-1] * up;

    #pragma omp parallel
    {
        int tid = omp_get_thread_num(); 
        int start = tid * thread_chunk; //we calculate the thread start position given the id of the thread 
        int end = (tid == max_threads - 1) ? N : (tid + 1) * thread_chunk - 1;
        
        double thread_start = Sn;
        //so, we calculate the start value of the thread according the start point of each thread but by our big chunk jumps
        for(int i = 0; i < start/CHUNK_SIZE; i++) {
            thread_start *= chunk_multiplier;  // Using chunk_multiplier directly since it's the same value
        }
        //however, we could leave out a reminder part so: 
        int complete_chunks = (end - start + 1) / CHUNK_SIZE; //chunks
        int remainder = (end - start + 1) % CHUNK_SIZE; //remidner
        //we process the whole chunk
        for(int c = 0; c < complete_chunks; c++) {
            for(int i = 0; i < CHUNK_SIZE; i++) {
                opt[start + i] = thread_start * chunk[i];
            }
            thread_start *= chunk_multiplier;
            start += CHUNK_SIZE;
        }

        // handle the reminder part
        if(remainder > 0) {
            for(int i = 0; i < remainder; i++) {
                opt[start + i] = thread_start * chunk[i];
            }
        }
        
        // since i get an error using last private in the context...
        //https://www.openmp.org/wp-content/uploads/OpenMP-API-Specification-5-2.pdf -> p.272  | order of assignment round robin on the number of threads. 
        if (tid == max_threads - 1) {
            Sn = opt[N];
        }
    }

    printf("Parallel RunTime  :  %f seconds\n", walltime() - time_start);
    printf("Final Result Sn   :  %.17g \n", Sn);

    double temp = 0.0;
    for (n = 0; n <= N; n++) {
        temp += opt[n] * opt[n];
    }
    
    printf("Result ||opt||^2_2 :  %f\n", temp / (double)N);
    printf("\n");

    free(chunk);
    free(opt);
    return 0;
}