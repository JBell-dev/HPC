#!/bin/bash  

#sources:
#https://docs.rc.uab.edu/cheaha/slurm/slurm_tutorial/
#https://slurm.schedmd.com/sbatch.html

#config: 
#SBATCH --job-name="printhost"
#SBATCH --nodes=2
#SBATCH --ntasks=2
#SBATCH --ntasks-per-node=1
#SBATCH --mem=16G
#SBATCH --output="helloworld-2nodes.txt"
#SBATCH --partition="slim"

# loading the used modules 
module load gcc
module list

# print CPU model
lscpu | grep "Model name"

#run
srun ./printhost #https://stackoverflow.com/questions/52117145/slurm-job-script-for-multiple-nodes

#command to check feature of the nodes: scontrol show nodes     sinfo
#NOTE THAT    AvailableFeatures=(null) so constraint will not help!!!
#sbatch printhostbatch2nodes.sh 
