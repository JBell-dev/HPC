#include <iostream>
#include <omp.h>
#include <random>
#include <vector>
#include "walltime.h"

#define VEC_SIZE 1000000000
#define BINS 16

int main() {
  double time_start, time_end;

  // Initialize random number generator
  unsigned int seed = 123;
  float mean = BINS / 2.0;
  float sigma = BINS / 12.0;
  std::default_random_engine generator(seed);
  std::normal_distribution<float> distribution(mean, sigma);

  // Generate random sequence
  // Note: normal distribution is on interval [-inf; inf]
  //       we want [0; BINS-1]
  std::vector<int> vec(VEC_SIZE);
  for (long i = 0; i < VEC_SIZE; ++i) {
    vec[i] = int(distribution(generator));
    if (vec[i] < 0       ) vec[i] = 0;
    if (vec[i] > BINS - 1) vec[i] = BINS - 1;
  }

  // Initialize histogram: Set all bins to zero
  long dist[BINS];
  for (int i = 0; i < BINS; ++i) {
    dist[i] = 0;
  }

  // TODO Parallelize the histogram computation
  time_start = walltime();
  #pragma omp parallel
  {
    //we create a private histogram for each thread -> since inside the parallel section then each thread has its own copy of the histogram
    long dist_private[BINS] = {0};  

     //we parallelize the for loop as always.
    #pragma omp for
    for (long i = 0; i < VEC_SIZE; ++i) {
      dist_private[vec[i]]++;
    }

    //now we agregate the results carefully as always.
    #pragma omp critical
    {
      for (int i = 0; i < BINS; ++i) {
        dist[i] += dist_private[i];
      }
    }
  }
  time_end = walltime();

  // Write results
  for (int i = 0; i < BINS; ++i) {
    std::cout << "dist[" << i << "]=" << dist[i] << std::endl;
  }
  std::cout << "Time: " << time_end - time_start << " sec" << std::endl;

  return 0;
}
