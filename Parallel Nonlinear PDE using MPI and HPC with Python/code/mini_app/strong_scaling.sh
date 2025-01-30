#!/bin/bash -l

#SBATCH --nodes=1
#SBATCH --ntasks=16
#SBATCH --ntasks-per-node=16
#SBATCH --cpus-per-task=1
#SBATCH --output=job.out
#SBATCH --error=job.err
#SBATCH --time=00:30:00

# Load required modules
module load openmpi

# Create CSV header with better formatting
echo "Grid_Size,Processes,CG_Iterations,Newton_Iterations,Runtime,Speedup,Efficiency" > strong_scaling.csv

# Test different grid sizes
for n in 64 128 256 512 1024; do
    echo -e "\n=== Testing grid size ${n}x${n} ==="
    
    # Get baseline time with 1 process
    echo "Running baseline with 1 process..."
    mpirun -np 1 ./main $n 100 0.01 | tee output_tmp
    baseline_time=$(grep "###" output_tmp | cut -d',' -f6 | tr -d ' ')
    baseline_cg=$(grep "###" output_tmp | cut -d',' -f4 | tr -d ' ')
    baseline_newton=$(grep "###" output_tmp | cut -d',' -f5 | tr -d ' ')
    
    # Test different numbers of processes
    for np in 1 2 4 8 16; do
        echo "Running with $np processes..."
        mpirun -np $np ./main $n 100 0.01 | tee output_tmp
        
        # Extract values
        procs=$(grep "###" output_tmp | cut -d',' -f1 | tr -d ' ')
        cg_iters=$(grep "###" output_tmp | cut -d',' -f4 | tr -d ' ')
        newton_iters=$(grep "###" output_tmp | cut -d',' -f5 | tr -d ' ')
        runtime=$(grep "###" output_tmp | cut -d',' -f6 | tr -d ' ')
        
        # Calculate metrics
        speedup=$(echo "scale=4; $baseline_time/$runtime" | bc)
        efficiency=$(echo "scale=4; $speedup/$np" | bc)
        
        # Save to CSV
        echo "$n,$np,$cg_iters,$newton_iters,$runtime,$speedup,$efficiency" >> strong_scaling.csv
        
        # Display results
        echo "Grid: ${n}x${n}, Processes: $np"
        echo "Runtime: $runtime s"
        echo "Speedup: $speedup"
        echo "Efficiency: $efficiency"
        echo "---"
    done
done

rm output_tmp