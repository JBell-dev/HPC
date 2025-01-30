#include <omp.h>
#include "walltime.h"
#include <iostream>
#include <math.h>
#include <stdio.h>
#include <unistd.h>

#define NUM_ITERATIONS 100

// Example benchmarks
// 0.008s ~0.8MB
//#define N 100000
// 0.1s ~8MB
// #define N 1000000
// 1.1s ~80MB
// #define N 10000000
// 13s ~800MB
// #define N 100000000
// 127s 16GB
//#define N 1000000000
#define EPSILON 0.1

using namespace std;

//https://learn.microsoft.com/en-us/cpp/cpp/main-function-command-line-args?view=msvc-170
int main(int argc, char* argv[]) {
  int N = atoi(argv[1]); //we pass the value of N as argument so we can pass from the loops of the bash
  double time_serial, time_start = 0.0;
  double *a, *b;

  // Allocate memory for the vectors as 1-D arrays
  a = new double[N];
  b = new double[N];

  // Initialize the vectors with some values
  for (int i = 0; i < N; i++) {
    a[i] = i;
    b[i] = i / 10.0;
  }
  long double alpha = 0;
  // serial execution
  // Note that we do extra iterations to reduce relative timing overhead
  time_start = wall_time();
  for (int iterations = 0; iterations < NUM_ITERATIONS; iterations++) {
    alpha = 0.0;
    for (int i = 0; i < N; i++) {
      alpha += (a[i]) * (b[i]);
    }
  }
  time_serial = wall_time() - time_start;
  //cout << "Serial execution time = " << time_serial << " sec" << endl;

  long double alpha_parallel = 0;
  long double alpha_parallel_critical = 0;
  double time_red = 0; // store the time taken by reduction method
  double time_critical = 0; // store the time taken by critical method

  //   TODO: Write parallel version (2 ways!)
  //   i.  Using reduction pragma
  time_start = wall_time();
  for (int iterations = 0; iterations < NUM_ITERATIONS; iterations++) {
    alpha_parallel = 0.0;
    #pragma omp parallel for default(none) shared(a, b, N) reduction(+:alpha_parallel)
    for (int i = 0; i < N; i++) {
      alpha_parallel += a[i] * b[i];
    }
  }
  time_red = wall_time() - time_start;
  
  // if ((fabs(alpha_parallel - alpha) / fabs(alpha_parallel)) > EPSILON) {
  //   cout << "parallel reduction: " << alpha_parallel << ", serial: " << alpha
  //        << "\n";
  //   cerr << "Alpha not yet implemented correctly!\n";
  //   exit(1);
  // }

   //   ii. Using  critical pragma
  time_start = wall_time();
  for (int iterations = 0; iterations < NUM_ITERATIONS; iterations++) {
    alpha_parallel_critical = 0.0;
    #pragma omp parallel default(none) shared(a, b, N, alpha_parallel_critical)
    {
      long double local_sum = 0.0; //each thread local sum
      #pragma omp for
      for (int i = 0; i < N; i++) {
        local_sum += a[i] * b[i];
      }
      //then we add them all together
      #pragma omp critical
      {
        alpha_parallel_critical += local_sum;
      }
    }
  }
  time_critical = wall_time() - time_start;


  // if ((fabs(alpha_parallel - alpha) / fabs(alpha_parallel)) > EPSILON) {
  //   cout << "parallel critical: " << alpha_parallel << ", serial: " << alpha
  //         << "\n";
  //   cerr << "Alpha not yet implemented correctly!\n";
  //   exit(1);
  // }

  // i commented out all the other results and output one so it is clean for generating the graph. -> base on how
  //was made the .data file on previous assignmetn for benchmarking
  printf("%d %d %.6f %.6f %.6f %.10Lf %.10Lf %.10Lf\n", N, omp_get_max_threads(), time_serial, time_red, time_critical, alpha_parallel,alpha_parallel_critical, alpha);
  //https://stackoverflow.com/questions/4264127/correct-format-specifier-for-double-in-printf

  // cout << "Parallel dot product = " << alpha_parallel
  //      << " time using reduction method = " << time_red
  //      << " sec, time using critical method " << time_critical << " sec"
  //      << endl;

  // De-allocate memory
  delete[] a;
  delete[] b;

  return 0;
}
