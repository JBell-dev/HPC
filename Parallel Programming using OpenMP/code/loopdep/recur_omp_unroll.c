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
    
    /* allocate memory for the recursion */
    double *opt = (double *)malloc((N + 1) * sizeof(double));
    if (opt == NULL) {
        perror("failed to allocate problem size");
        exit(EXIT_FAILURE);
    }

    double time_start = walltime();
    opt[0] = Sn;
    double up2 = up * up;
    double up3 = up2 * up;
    
    for (n = 0; n <= N - 3; n += 3) {
        opt[n+1] = opt[n] * up;
        opt[n+2] = opt[n] * up2;
        opt[n+3] = opt[n] * up3;
    }
    
    //if somthing left to compute 
    for (; n <= N; n++) {
        opt[n] = opt[n-1] * up;
    }

    Sn = opt[N]; 

    printf("RunTime           :  %f seconds\n", walltime() - time_start);
    printf("Final Result Sn   :  %.17g \n", Sn);

    double temp = 0.0;
    for (n = 0; n <= N; ++n) {
        temp += opt[n] * opt[n];
    }
    printf("Result ||opt||^2_2 :  %f\n", temp / (double)N);
    printf("\n");

    free(opt);
    return 0;
}