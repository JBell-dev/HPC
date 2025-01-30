#!/bin/bash  

#sources:
#https://docs.rc.uab.edu/cheaha/slurm/slurm_tutorial/
#https://slurm.schedmd.com/sbatch.html

#config: 
#SBATCH --job-name="printhost"
#SBATCH --nodes=1                       # required only one for now
#SBATCH --mem=16G
#SBATCH --partition="bigMem"
#SBATCH --output="helloworld1.txt"
#SBATCH --partition="slim"

# loading the used modules 
module load gcc
module list

# print CPU model
lscpu | grep "Model name"

#run
./printhost

#command to check feature of the nodes: scontrol show nodes     sinfo
#NOTE THAT    AvailableFeatures=(null) so constraint will not help!!!
