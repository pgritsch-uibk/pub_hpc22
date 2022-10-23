#!/bin/bash

# Execute job in the partition "lva" unless you have special requirements.
#SBATCH --partition=lva
# Name your job to be able to identify it later
#SBATCH --job-name mc_philipp
# Redirect output stream to this file
#SBATCH --output=output_philipp_mc_2.log
# Enforce exclusive node allocation, do not share with other jobs
#SBATCH --exclusive


perf stat ./monte_carlo_seq
perf stat ./monte_carlo_seq
perf stat ./monte_carlo_seq
perf stat ./monte_carlo_seq
perf stat ./monte_carlo_seq
perf stat ./monte_carlo_seq
perf stat ./monte_carlo_seq
perf stat ./monte_carlo_seq
perf stat ./monte_carlo_seq
perf stat ./monte_carlo_seq
perf stat ./monte_carlo_seq
