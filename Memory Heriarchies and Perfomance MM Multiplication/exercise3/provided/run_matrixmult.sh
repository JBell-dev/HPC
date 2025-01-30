#!/bin/bash
#SBATCH --job-name=matrixmult
#SBATCH --output=matrixmult-%j.out
#SBATCH --error=matrixmult-%j.err
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=1
#SBATCH --partition=slim
#SBATCH --time=00:20:00

# load some modules & list loaded modules
module list

# set some environment variables
export OMP_NUM_THREADS=1

echo "==== benchmark-naive ======================"
srun ./benchmark-naive | tee timing_basic_dgemm.data
echo
echo "==== benchmark-blas ======================="
srun ./benchmark-blas | tee timing_blas_dgemm.data
echo
echo "==== benchmark-blocked ===================="
srun ./benchmark-blocked | tee timing_blocked_dgemm.data

echo
echo "==== plot results ========================="
gnuplot timing.gp
