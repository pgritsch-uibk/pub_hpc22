#!/bin/bash

for k in 10000 100000 1000000 10000000 100000000 1000000000
do
    for j in 1 2 4 8
    do
        for i in 1 2 3 4
        do
            sbatch --output=montecarlo_pi_two_${k}_${j}_${i}.log --ntasks=$j chapel_montecarlo_two.sh $k
        done
    done
done

for k in 10000 100000 1000000 10000000 100000000 1000000000
do
    for j in 1 2 4 8
    do
        for i in 1 2 3 4
        do
            sbatch --output=montecarlo_pi_${k}_${j}_${i}.log --ntasks=$j chapel_montecarlo.sh $k
        done
    done
done
