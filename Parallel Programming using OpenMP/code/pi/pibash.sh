#!/bin/bash
#SBATCH --job-name=piApproximation
#SBATCH --output=piApproximation-%j.out
#SBATCH --error=piApproximation-%j.err
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=20
#SBATCH --partition=slim
#SBATCH --time=02:00:00
#sources: https://stackoverflow.com/questions/4662938/create-text-file-and-fill-it-using-bash
# 
module load gcc
module list
make

thread_counts=(1 2 4 8)

#we create/overwrite the file with the respective columns name
echo "# Threads Serial_Time Parallel_Time Serial_Pi Parallel_Pi" > pi_approximation_results.data

for T in "${thread_counts[@]}"; do
    echo "Running with T = $T threads"
    export OMP_NUM_THREADS=$T
    output=$(./pi) #we store the output : https://unix.stackexchange.com/questions/440088/what-is-command-substitution-in-a-shell
    #https://stackoverflow.com/questions/17418546/how-to-return-the-output-of-program-in-a-variable
    #https://stackoverflow.com/questions/25535642/how-do-i-store-the-output-of-a-bash-command-in-a-variable/25535724
    #https://www.baeldung.com/linux/grep-bash-variable
    #we search in the variable output the specific pattern name 
    serial_time=$(echo "$output" | grep "time_srl")
    parallel_time=$(echo "$output" | grep "time_plr")
    serial_pi=$(echo "$output" | grep "time_srl")
    parallel_pi=$(echo "$output" | grep "time_plr")
    
    echo "$T $serial_time $parallel_time $serial_pi $parallel_pi" >> pi_approximation_results.data
done

