#!/bin/bash


# Execute job in the partition "lva" unless you have special requirements.
#SBATCH --partition=lva
# Name your job to be able to identify it later
#SBATCH --job-name heat_stencil_2D
# Redirect output stream to this file
##SBATCH --output=output_philipp_hs_mpi.log
# Maximum number of tasks (=processes) to start in total
#SBATCH --ntasks=16
# Enforce exclusive node allocation, do not share with other jobs
#SBATCH --exclusive

module load openmpi/4.0.3

perf stat mpiexec -n $SLURM_NTASKS ./build/hw03/parallel/3_heat_stencil_2d_par 1920
perf stat mpiexec -n $SLURM_NTASKS ./build/hw03/parallel/3_heat_stencil_2d_par 1920