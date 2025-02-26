#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include "consts.h"
#include "pngwriter.h"
#include "walltime.h"

int main(int argc, char **argv) {
  png_data *pPng = png_create(IMAGE_WIDTH, IMAGE_HEIGHT);

  double x, y, x2, y2, cx, cy;

  double fDeltaX = (MAX_X - MIN_X) / (double)IMAGE_WIDTH;
  double fDeltaY = (MAX_Y - MIN_Y) / (double)IMAGE_HEIGHT;

  long nTotalIterationsCount = 0;

  long i, j;

  double time_start = walltime();
  // do the calculation
  #pragma omp parallel reduction(+:nTotalIterationsCount) private(x, y, x2, y2, cx, cy, i)
  {
    #pragma omp for schedule(dynamic)
    for (j = 0; j < IMAGE_HEIGHT; j++) {
      cy = MIN_Y + j * fDeltaY;
      for (i = 0; i < IMAGE_WIDTH; i++) {
        cx = MIN_X + i * fDeltaX;
        x = cx;
        y = cy;
        x2 = x * x;
        y2 = y * y;
        // compute the orbit z, f(z), f^2(z), f^3(z), ...
        // count the iterations until the orbit leaves the circle |z|=2.
        // stop if the number of iterations exceeds the bound MAX_ITERS.
        int n = 0;
        while (x2 + y2 < 4.0 && n < MAX_ITERS) { // |z| < 2.0 the point should lay on the 2 unit circle of complex plane
        //| z | = |x + yi| = sqrt(x^2 + y^2) < 2 = x^2 + y^2 < 4
        //since z = x + iy, z^2 = x^2 - y^2 + 2ixy then the new x = x^2 - y^2 + cx with the given c = cx + cyi
          y = 2 * x * y + cy;
          x = x2 - y2 + cx;
          x2 = x * x;
          y2 = y * y;
          n++;
        }
        nTotalIterationsCount += n;
        // n indicates if the point belongs to the mandelbrot set
        // plot the number of iterations at point (i, j)
        int c = ((long)n * 255) / MAX_ITERS;
        #pragma omp critical
        {
          png_plot(pPng, i, j, c, c, c);
        }
      }
    }
  }
  double time_end = walltime();

  // print benchmark data
  printf("Total time:                 %g seconds\n",
         (time_end - time_start));
  printf("Image size:                 %ld x %ld = %ld Pixels\n",
         (long)IMAGE_WIDTH, (long)IMAGE_HEIGHT,
         (long)(IMAGE_WIDTH * IMAGE_HEIGHT));
  printf("Total number of iterations: %ld\n", nTotalIterationsCount);
  printf("Avg. time per pixel:        %g seconds\n",
         (time_end - time_start) / (double)(IMAGE_WIDTH * IMAGE_HEIGHT));
  printf("Avg. time per iteration:    %g seconds\n",
         (time_end - time_start) / (double)nTotalIterationsCount);
  printf("Iterations/second:          %g\n",
         nTotalIterationsCount / (time_end - time_start));
  // assume there are 8 floating point operations per iteration
  printf("MFlop/s:                    %g\n",
         nTotalIterationsCount * 8.0 / (time_end - time_start) * 1.e-6);

  png_write(pPng, "mandel.png");
  return 0;
}