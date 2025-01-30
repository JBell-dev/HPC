#!/bin/bash -l

#SBATCH --nodes=20         
#SBATCH --ntasks=20        
#SBATCH --ntasks-per-node=1 
#SBATCH --cpus-per-task=1   
#SBATCH --output=job.out
#SBATCH --error=job.err
#SBATCH --time=00:20:00

echo "processes,size,iterations,eigenvalue,runtime" > weak_scaling.csv

echo -e "\nWeak scaling..."

declare -A matrix_sizes=(
    [1]=10000
    [2]=14142   
    [4]=20000   
    [8]=28284    
    [16]=40000  
    [20]=44721   
)

echo "Running with 1 process:"
srun -N 1 -n 1 ./powermethod_rows 3 ${matrix_sizes[1]} 300 -1e-6 | tee output_tmp
baseline_time=$(grep "###" output_tmp | cut -d',' -f5)

for np in 1 2 4 8 16 20; do
    size=${matrix_sizes[$np]}
    echo "Running with $np processes, matrix size $size:"
    # Use srun with explicit node count to ensure one process per node
    srun -N $np -n $np ./powermethod_rows 3 $size 300 -1e-6 | tee output_tmp
    values=$(grep "###" output_tmp | tr -d '#' | tr -d ' ')
    echo "$values" >> weak_scaling.csv
    
    time=$(echo $values | cut -d',' -f5)
    efficiency=$(echo "scale=4; $baseline_time/$time" | bc)
    echo "Weak Scaling Efficiency: $efficiency"
done
rm output_tmp