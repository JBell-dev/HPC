#!/bin/bash
#SBATCH --job-name=Mandel
#SBATCH --output=Mandel-%j.out
#SBATCH --error=Mandel-%j.err
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=20
#SBATCH --partition=slim
#SBATCH --time=04:00:00

module load gcc
module list
make

thread=(1 2 4 8 16 20)

#we intialize the files we need
data_file="mandelbrot_results.data"
full_output_file="mandelbrot_full_output.txt"
echo "# Threads Total_Time Iterations_per_Second MFlops" > $data_file
> $full_output_file

for T in "${thread[@]}"; do
    echo "Running with T = $T threads" | tee -a $full_output_file
    export OMP_NUM_THREADS=$T
    
    # the full output if required
    output=$(./mandel_seq | tee -a $full_output_file)
    echo "----------------------------------------" >> $full_output_file
    
    #for our .data
    total_time=$(echo "$output" | grep "Total time:" | awk '{print $3}')
    iterations_per_second=$(echo "$output" | grep "Iterations/second:" | awk '{print $2}')
    mflops=$(echo "$output" | grep "MFlop/s:" | awk '{print $2}')
    echo "$T $total_time $iterations_per_second $mflops" >> $data_file
    
    
done

gnuplot mandelplot.gp