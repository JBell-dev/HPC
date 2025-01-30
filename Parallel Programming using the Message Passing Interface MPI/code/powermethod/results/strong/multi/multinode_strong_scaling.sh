#!/bin/bash -l

#SBATCH --nodes=19  
#SBATCH --ntasks=19
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=1
#SBATCH --output=job.out
#SBATCH --error=job.err
#SBATCH --time=00:20:00


echo "processes,size,iterations,eigenvalue,runtime" > strong_scaling.csv

echo -e "\nStrong scaling..."
#single proc for baseline time
echo "Running with 1 process:"
mpirun -np 1 ./powermethod_rows 3 10000 300 -1e-6 | tee output_tmp
baseline_time=$(grep "###" output_tmp | cut -d',' -f5)

for np in 1 2 4 8 16 20; do
    echo "Running with $np processes:"
    mpirun -np $np ./powermethod_rows 3 10000 300 -1e-6 | tee output_tmp
    #to csv
    values=$(grep "###" output_tmp | tr -d '#' | tr -d ' ')
    echo "$values" >> strong_scaling.csv
    
    # efficiency calculation
    time=$(echo $values | cut -d',' -f5)
    efficiency=$(echo "scale=4; $baseline_time/($np*$time)" | bc)
    echo "Efficiency: $efficiency"
done
rm output_tmp