#!/bin/bash
#SBATCH --job-name=dotProduct
#SBATCH --output=dotProduct-%j.out
#SBATCH --error=dotProduct-%j.err
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=20
#SBATCH --partition=slim
#SBATCH --time=02:00:00

#base on previous assignment "run_matrixmult.sh":
# load modules
module load gcc
module list
# compile
make


# inputs 100000000 1000000000
N_values=(100000 1000000 10000000)
thread_counts=(1 2 4 8 16 20)

# experiments
#https://www.geeksforgeeks.org/bash-scripting-for-loop/
#https://www.hostinger.com/tutorials/bash-for-loop-guide-and-examples/
> dotproduct_results.data
for N in "${N_values[@]}"; do
  for T in "${thread_counts[@]}"; do
    echo "Running with N = $N and T = $T threads"
    #we set the threads to be used
    export OMP_NUM_THREADS=$T
    srun ./dotProduct $N >> dotproduct_results.data 
    #https://stackoverflow.com/questions/11162406/open-and-write-data-to-text-file-using-bash
  done
done

#gnuplot plotted.gp -> i tried to code the gnuplot to run it directly but i could not add the legends with the different n values

