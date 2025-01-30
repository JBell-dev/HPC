#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "walltime.h"
#include <omp.h>

int main(int argc, char *argv[]) {
  int N =2000000000;
  double up = 1.00000001;
  double Sn = 1.00000001;
  int n;

  //TESTING
  int num_threads = 3;
  int count = 0;
  omp_set_num_threads(num_threads);

  /* allocate memory for the recursion */
  double *opt = (double *)malloc((N + 1) * sizeof(double));
  if (opt == NULL) {
    perror("failed to allocate problem size");
    exit(EXIT_FAILURE);
  }

  double time_start = walltime();
  #pragma omp parallel for firstprivate(Sn,count) lastprivate(Sn) 
  for (n = 0; n <= N; ++n) {
    if (count == 0) {
        Sn = Sn * pow(up, n);
        count++;
    }
    opt[n] = Sn;
    Sn *= up;
  }

  printf("Parallel RunTime  :  %f seconds\n", walltime() - time_start);
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