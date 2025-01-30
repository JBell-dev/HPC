#!/bin/bash -l

#SBATCH --nodes=1
#SBATCH --ntasks=17      #16 workers + 1 manager
#SBATCH --ntasks-per-node=17
#SBATCH --cpus-per-task=1
#SBATCH --output=scaling_study.out
#SBATCH --error=scaling_study.err
#SBATCH --time=00:30:00
conda activate project5_env



domX=4001
domY=4001
echo "workers,tasks,runtime" > scaling_results.csv

for tasks in 50 100; do
    for workers in 2 4 8 16; do
        total_procs=$((workers + 1))
        echo "Running with $workers workers and $tasks tasks..."
        runtime=$(mpiexec -n $total_procs python manager_worker.py $domX $domY $tasks | grep "Run took" | awk '{print $3}')
        echo "$workers,$tasks,$runtime" >> scaling_results.csv
    done
done
