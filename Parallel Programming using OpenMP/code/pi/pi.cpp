#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

// since f(x) = 4/(1+x^2)
double f(double x) {
    return 4.0 / (1.0 + x * x);
}

int main(int argc, char *argv[]) {
  long int N = 10000000000; // <<< carefull! This larger than regular int (its a long int)
  double dx = 1./double(N); //size of the subinterval
  double sum=0;
  double pi =0;
  double time=0;
  

  /* TODO Serial Version of Computing Pi*/
  time = omp_get_wtime();
  for (long int i = 0; i < N; i++) {
      double x = (i + 0.5) * dx; //x as in assignment 
      sum += f(x);
  }
  pi = sum * dx;
  time = omp_get_wtime() - time;
  printf("pi=%e, N=%ld, time_srl=%e secs\n", pi, N, time);


  /* TODO Parallel Version of Computing Pi*/
  time = omp_get_wtime();
  sum = 0.0;
  // TODO Fill this gap
  #pragma omp parallel for reduction(+:sum)
  for (long int i = 0; i < N; i++) {
      double x = (i + 0.5) * dx;
      sum += f(x);
  }
  pi = sum*dx;
  time = omp_get_wtime() - time;
  printf("pi=%e, N=%ld, time_plr=%e secs\n", pi, N, time);

  return 0;
}
