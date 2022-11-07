#!/bin/bash

./compile.sh

for i in 1 2
do
    sbatch --output=output_philipp_hs_parallel_256_$i.log job_parallel.sh 
done

for i in 1 2
do
    sbatch --output=output_philipp_hs_parallel_single_256_$i.log job_parallel_single.sh 
done

for i in 1 2
do
    sbatch --output=output_philipp_hs_sequential_256_$i.log job_sequential.sh 
done
