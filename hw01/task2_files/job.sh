#!/bin/bash

# Execute job in the partition "lva" unless you have special requirements.
#SBATCH --partition=lva
# Name your job to be able to identify it later
#SBATCH --job-name osu_bw_philipp
# Redirect output stream to this file
#SBATCH --output=output_bw_22_spn1_nps1.log
# Maximum number of tasks (=processes) to start in total
#SBATCH --ntasks=2
# Maximum number of tasks (=processes) to start per node
#SBATCH --ntasks-per-node=2
# Enforce exclusive node allocation, do not share with other jobs
#SBATCH --exclusive

#SBATCH --sockets-per-node=1
#SBATCH --ntasks-per-socket=1

module load openmpi/4.0.3

mpiexec -n $SLURM_NTASKS /home/cb76/cb761015/osu-installation/libexec/osu-micro-benchmarks/mpi/pt2pt/osu_bw
 
