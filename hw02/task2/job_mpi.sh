#!/bin/bash

# Execute job in the partition "lva" unless you have special requirements.
#SBATCH --partition=lva
# Name your job to be able to identify it later
#SBATCH --job-name mc_mpi_philipp
# Redirect output stream to this file
#SBATCH --output=output_philipp_hs_mpi.log
# Maximum number of tasks (=processes) to start in total
#SBATCH --ntasks=64
# Enforce exclusive node allocation, do not share with other jobs
#SBATCH --exclusive

module load openmpi/4.0.3

perf stat mpiexec -n $SLURM_NTASKS ./heat_stencil_1D_par 1920
perf stat mpiexec -n $SLURM_NTASKS ./heat_stencil_1D_par 1920
perf stat mpiexec -n $SLURM_NTASKS ./heat_stencil_1D_par 1920
perf stat mpiexec -n $SLURM_NTASKS ./heat_stencil_1D_par 1920
perf stat mpiexec -n $SLURM_NTASKS ./heat_stencil_1D_par 1920
perf stat mpiexec -n $SLURM_NTASKS ./heat_stencil_1D_par 4096
perf stat mpiexec -n $SLURM_NTASKS ./heat_stencil_1D_par 4096
perf stat mpiexec -n $SLURM_NTASKS ./heat_stencil_1D_par 4096
perf stat mpiexec -n $SLURM_NTASKS ./heat_stencil_1D_par 4096
perf stat mpiexec -n $SLURM_NTASKS ./heat_stencil_1D_par 4096
perf stat mpiexec -n $SLURM_NTASKS ./heat_stencil_1D_par 8192
perf stat mpiexec -n $SLURM_NTASKS ./heat_stencil_1D_par 8192
perf stat mpiexec -n $SLURM_NTASKS ./heat_stencil_1D_par 8192
perf stat mpiexec -n $SLURM_NTASKS ./heat_stencil_1D_par 8192
perf stat mpiexec -n $SLURM_NTASKS ./heat_stencil_1D_par 8192
perf stat mpiexec -n $SLURM_NTASKS ./heat_stencil_1D_par 16384
perf stat mpiexec -n $SLURM_NTASKS ./heat_stencil_1D_par 16384
perf stat mpiexec -n $SLURM_NTASKS ./heat_stencil_1D_par 16384
perf stat mpiexec -n $SLURM_NTASKS ./heat_stencil_1D_par 16384
perf stat mpiexec -n $SLURM_NTASKS ./heat_stencil_1D_par 16384
perf stat mpiexec -n $SLURM_NTASKS ./heat_stencil_1D_par 32768
perf stat mpiexec -n $SLURM_NTASKS ./heat_stencil_1D_par 32768
perf stat mpiexec -n $SLURM_NTASKS ./heat_stencil_1D_par 32768
perf stat mpiexec -n $SLURM_NTASKS ./heat_stencil_1D_par 32768
perf stat mpiexec -n $SLURM_NTASKS ./heat_stencil_1D_par 32768