#!/bin/bash -l

#SBATCH --nodes=1
#SBATCH --ntasks=16
#SBATCH --ntasks-per-node=16
#SBATCH --cpus-per-task=1
#SBATCH --output=job.out
#SBATCH --error=job.err
#SBATCH --time=00:30:00

module load openmpi
echo "Grid_Size,Processes,CG_Iterations,Newton_Iterations,Runtime" > weak_scaling.csv

declare -a sizes1=(64 90 128 181 256)
declare -a sizes2=(128 181 256 362 512)
declare -a sizes3=(256 362 512 724 1024)
declare -a procs=(1 2 4 8 16)

#base size 64
echo "Running base size 64x64..."
for i in {0..4}; do
    np=${procs[$i]}
    n=${sizes1[$i]}
    
    echo "Running with $np processes, grid size ${n}x${n}..."
    mpirun -np $np ./main $n 100 0.01 | tee output_tmp

    cg_iters=$(grep "###" output_tmp | cut -d',' -f4 | tr -d ' ')
    newton_iters=$(grep "###" output_tmp | cut -d',' -f5 | tr -d ' ')
    runtime=$(grep "###" output_tmp | cut -d',' -f6 | tr -d ' ')
    echo "$n,$np,$cg_iters,$newton_iters,$runtime" >> weak_scaling.csv
    echo "Grid: ${n}x${n}, Processes: $np"
    echo "Runtime: $runtime s"
    echo "---"
done

#same base size 128
echo "Running base size 128x128..."
for i in {0..4}; do
    np=${procs[$i]}
    n=${sizes2[$i]}
    
    echo "Running with $np processes, grid size ${n}x${n}..."
    mpirun -np $np ./main $n 100 0.01 | tee output_tmp
    
    cg_iters=$(grep "###" output_tmp | cut -d',' -f4 | tr -d ' ')
    newton_iters=$(grep "###" output_tmp | cut -d',' -f5 | tr -d ' ')
    runtime=$(grep "###" output_tmp | cut -d',' -f6 | tr -d ' ')
    echo "$n,$np,$cg_iters,$newton_iters,$runtime" >> weak_scaling.csv
    echo "Grid: ${n}x${n}, Processes: $np"
    echo "Runtime: $runtime s"
    echo "---"
done

# 256
echo "Running base size 256x256..."
for i in {0..4}; do
    np=${procs[$i]}
    n=${sizes3[$i]}
    
    echo "Running with $np processes, grid size ${n}x${n}..."
    mpirun -np $np ./main $n 100 0.01 | tee output_tmp
    
    cg_iters=$(grep "###" output_tmp | cut -d',' -f4 | tr -d ' ')
    newton_iters=$(grep "###" output_tmp | cut -d',' -f5 | tr -d ' ')
    runtime=$(grep "###" output_tmp | cut -d',' -f6 | tr -d ' ')
    echo "$n,$np,$cg_iters,$newton_iters,$runtime" >> weak_scaling.csv
    echo "Grid: ${n}x${n}, Processes: $np"
    echo "Runtime: $runtime s"
    echo "---"
done

rm output_tmp