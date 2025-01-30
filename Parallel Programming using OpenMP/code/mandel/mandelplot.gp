set terminal png size 800,600
set output 'mandelbrot_performance.png'
set title 'Mandelbrot Set Performance'
set xlabel 'Number of Threads'
set ylabel 'Performance'
set key outside
set logscale x 2
plot 'mandelbrot_results.data' using 1:4 with linespoints title 'MFlop/s'