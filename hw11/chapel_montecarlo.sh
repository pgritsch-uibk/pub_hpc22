#!/bin/bash


# Execute job in the partition "lva" unless you have special requirements.
#SBATCH --partition=lva
# Name your job to be able to identify it later
#SBATCH --job-name test
# Redirect output stream to this file
##SBATCH --output=montecarlo.log
# Maximum number of tasks (=processes) to start in total
##SBATCH --ntasks=8
# Enforce exclusive node allocation, do not share with other jobs
#SBATCH --exclusive

export CHPL_TASKS=qthreads
export CHPL_RT_NUM_THREADS_PER_LOCALE=$SLURM_NTASKS

./montecarlo --n=${1} 