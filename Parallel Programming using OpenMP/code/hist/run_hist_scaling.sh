#!/bin/bash
#SBATCH --job-name=hist_scaling      # Job name
#SBATCH --output=hist_scaling-%j.out # Output file
#SBATCH --error=hist_scaling-%j.err  # Error file
#SBATCH --ntasks=1                   # Number of tasks
#SBATCH --cpus-per-task=20           # Number of CPUs per task
#SBATCH --mem-per-cpu=1024           # Memory per CPU
#SBATCH --time=00:05:00              # Wall clock time limit

# Load some modules & list loaded modules
module load gcc
module list

# Compile
make clean
make

# OpenMP settings
export OMP_PROC_BIND=true

# Strong scaling
echo "=== Strong scaling ========================"
STRONG_SCALING_DATA=hist_omp_strong_scaling.data
> $STRONG_SCALING_DATA
for ((i=0; i<=7; i++))
do
  OMP_NUM_THREADS=$((2**i))
  export OMP_NUM_THREADS
  ./hist_omp | tee -a $STRONG_SCALING_DATA
done
