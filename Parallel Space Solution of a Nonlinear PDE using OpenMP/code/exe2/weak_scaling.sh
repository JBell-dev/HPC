#!/bin/bash
#SBATCH --nodes=1
#SBATCH --exclusive
#SBATCH --time=01:00:00
#SBATCH --output=weak_scaling_%j.out
#SBATCH --error=weak_scaling_%j.err
#SBATCH --job-name=weak_scaling

# parameters
BASE_SIZES=(64 128 256)    
THREADS=(1 2 4 8 16)    
TIMESTEPS=100              
FINAL_TIME=0.005          
OUTPUT_CSV="weak_scaling_results.csv" 

calculate_size() {
    #sources: https://ryanstutorials.net/bash-scripting-tutorial/bash-functions.php
    # https://stackoverflow.com/questions/12722095/how-do-i-use-floating-point-arithmetic-in-bash
    #https://www.unix.com/unix-for-dummies-questions-and-answers/152680-sqrt-bash.html
    #https://linuxize.com/post/bash-case-statement/
    local base=$1
    local threads=$2
    case $threads in
        1) echo "$base";;  # n = base
        2) echo "scale=0; ($base * 1.4142 + 0.5)" | bc;;  
        4) echo "scale=0; ($base * 2 + 0.5)" | bc;;       
        8) echo "scale=0; ($base * 2.8284 + 0.5)" | bc;; 
        16) echo "scale=0; ($base * 4 + 0.5)" | bc;;      
    esac
}

echo "Compiling..."
make clean
make

echo "base_resolution,threads,grid_size,time,iters,newton_iter,work_per_thread" > $OUTPUT_CSV
for base_size in "${BASE_SIZES[@]}"; do
    echo "Processing base resolution ${base_size}x${base_size}"
    for threads in "${THREADS[@]}"; do
        grid_size=$(calculate_size $base_size $threads)
        
        echo "Running ${grid_size}x${grid_size} grid on ${threads} threads..."
        export OMP_NUM_THREADS=$threads

        output=$(./main $grid_size $TIMESTEPS $FINAL_TIME | grep "###")
        time=$(echo $output | tr ',' ' ' | awk '{print $7}')
        iters=$(echo $output | tr ',' ' ' | awk '{print $5}')
        newton_iter=$(echo $output | tr ',' ' ' | awk '{print $6}')
        work_per_thread=$(echo "scale=1; $grid_size * $grid_size / $threads" | bc)
        
        echo "$base_size,$threads,$grid_size,$time,$iters,$newton_iter,$work_per_thread" >> $OUTPUT_CSV
    done
done

