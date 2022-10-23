#!/bin/bash

# Execute job in the partition "lva" unless you have special requirements.
#SBATCH --partition=lva
# Name your job to be able to identify it later
#SBATCH --job-name mc_mpi_philipp
# Redirect output stream to this file
#SBATCH --output=output_philipp_mc_mpi_2.log
# Maximum number of tasks (=processes) to start in total
#SBATCH --ntasks=64
# Maximum number of tasks (=processes) to start per node
##SBATCH --ntasks-per-node=8
# Enforce exclusive node allocation, do not share with other jobs
#SBATCH --exclusive

module load openmpi/4.0.3

perf stat mpiexec -n $SLURM_NTASKS ./monte_carlo_mpi
perf stat mpiexec -n $SLURM_NTASKS ./monte_carlo_mpi
perf stat mpiexec -n $SLURM_NTASKS ./monte_carlo_mpi
perf stat mpiexec -n $SLURM_NTASKS ./monte_carlo_mpi
perf stat mpiexec -n $SLURM_NTASKS ./monte_carlo_mpi
perf stat mpiexec -n $SLURM_NTASKS ./monte_carlo_mpi
perf stat mpiexec -n $SLURM_NTASKS ./monte_carlo_mpi
perf stat mpiexec -n $SLURM_NTASKS ./monte_carlo_mpi
perf stat mpiexec -n $SLURM_NTASKS ./monte_carlo_mpi
perf stat mpiexec -n $SLURM_NTASKS ./monte_carlo_mpi
perf stat mpiexec -n $SLURM_NTASKS ./monte_carlo_mpi
