#!/bin/bash

# Execute job in the partition "lva" unless you have special requirements.
#SBATCH --partition=lva
# Name your job to be able to identify it later
#SBATCH --job-name mc_philipp
# Redirect output stream to this file
#SBATCH --ntasks=1
#SBATCH --output=output_philipp_hs.log
# Enforce exclusive node allocation, do not share with other jobs
#SBATCH --exclusive


perf stat ./heat_stencil_1D_seq 1920
perf stat ./heat_stencil_1D_seq 1920
perf stat ./heat_stencil_1D_seq 1920
perf stat ./heat_stencil_1D_seq 1920
perf stat ./heat_stencil_1D_seq 1920

perf stat ./heat_stencil_1D_seq 4096
perf stat ./heat_stencil_1D_seq 4096
perf stat ./heat_stencil_1D_seq 4096
perf stat ./heat_stencil_1D_seq 4096
perf stat ./heat_stencil_1D_seq 4096

perf stat ./heat_stencil_1D_seq 8192
perf stat ./heat_stencil_1D_seq 8192
perf stat ./heat_stencil_1D_seq 8192
perf stat ./heat_stencil_1D_seq 8192
perf stat ./heat_stencil_1D_seq 8192
