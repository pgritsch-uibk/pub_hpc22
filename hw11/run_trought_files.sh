#!/bin/bash

for k in 10000 100000 1000000 10000000 100000000 1000000000
do
    for j in 1 2 4 8
    do
        echo -n "Parallel ${k} ${j} "
        ./test.sh montecarlo_pi_two_${k}_${j}_*.log
    done
done

for k in 10000 100000 1000000 10000000 100000000 1000000000
do
    for j in 1 2 4 8
    do
        echo -n "Parallel Two ${k} ${j} "
        ./test.sh montecarlo_pi_${k}_${j}_*.log
    done
done


# for i in 512 1024 2048 4096
# do
#     for j in 16 64 
#     do
#         echo -n "Parallel ${i} ${j} "
#         ./test.sh output_parallel_${i}_${j}_*.log 

#         echo -n "Parallel nb ${i} ${j} "
#         ./test.sh output_parallel_non_${i}_${j}_*.log 
#     done
# done


# for i in 512 1024 2048 4096
# do
#     echo -n "Sequential ${i} "
#     ./test.sh output_sequential_${i}*.log 
# done

