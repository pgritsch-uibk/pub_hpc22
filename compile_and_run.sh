#!/bin/bash

./compile.sh

for i in 1 2
do
    sbatch --output=output_parallel_$i.log job_parallel.sh 
done

# for i in 1 2
# do
#     sbatch --output=output_single_$i.log job_parallel_single.sh 
# done

# for i in 1 2
# do
#     sbatch --output=output_sequential_$i.log job_sequential.sh 
# done
