#!/bin/bash
#SBATCH --nodes=1
#SBATCH --exclusive
#SBATCH --time=01:00:00
#SBATCH --output=scaling_%j.out
#SBATCH --error=scaling_%j.err
#SBATCH --job-name=scaling_analysis

# parameters:
SIZES=(64 128 256 512 1024)    
THREADS=(1 2 4 8 16)           
TIMESTEPS=100                   
FINAL_TIME=0.005                 
OUTPUT_CSV="scaling_results.csv" 

echo "Compiling..."
make clean
make

echo "size,threads,time,iters,newton_iter" > $OUTPUT_CSV
for size in "${SIZES[@]}"; do
    for threads in "${THREADS[@]}"; do
        echo "Running size ${size}x${size} with ${threads} threads..."
        export OMP_NUM_THREADS=$threads
        output=$(./main $size $TIMESTEPS $FINAL_TIME | grep "###")
        # "### threads, nx, nt, iters_cg, iters_newton, time "
        time=$(echo "$output" | tr -d '###' | tr ',' ' ' | awk '{print $6}')
        iters=$(echo "$output" | tr -d '###' | tr ',' ' ' | awk '{print $4}')
        newton_iter=$(echo "$output" | tr -d '###' | tr ',' ' ' | awk '{print $5}')
        
        # saving to csv
        echo "$size,$threads,$time,$iters,$newton_iter" >> $OUTPUT_CSV
    done
done

#sources:
#https://medium.com/@amankroy98/arrays-for-bash-shell-scripting-867ee2d3add0#:~:text=Bash%20shell%20array%20is%20zero,use%20%24%7Barrayname%5B0%5D%7D%20.