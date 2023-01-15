#!/bin/bash


sudo perf stat -a -r 5 -e "power/energy-pkg/","power/energy-cores/","power/energy-ram/" mpiexec --oversubscribe -n 1 --allow-run-as-root  ./main_par 1920 1080
sudo perf stat -a -r 5 -e "power/energy-pkg/","power/energy-cores/","power/energy-ram/" mpiexec --oversubscribe -n 1 --allow-run-as-root  ./main_par 3840 2176
sudo perf stat -a -r 5 -e "power/energy-pkg/","power/energy-cores/","power/energy-ram/" mpiexec --oversubscribe -n 1 --allow-run-as-root  ./main_par 7680 4352

sudo perf stat -a -r 5 -e "power/energy-pkg/","power/energy-cores/","power/energy-ram/" mpiexec --oversubscribe -n 2 --allow-run-as-root  ./main_par 1920 1080
sudo perf stat -a -r 5 -e "power/energy-pkg/","power/energy-cores/","power/energy-ram/" mpiexec --oversubscribe -n 2 --allow-run-as-root  ./main_par 3840 2176
sudo perf stat -a -r 5 -e "power/energy-pkg/","power/energy-cores/","power/energy-ram/" mpiexec --oversubscribe -n 2 --allow-run-as-root  ./main_par 7680 4352

sudo perf stat -a -r 5 -e "power/energy-pkg/","power/energy-cores/","power/energy-ram/" mpiexec --oversubscribe -n 4 --allow-run-as-root  ./main_par 1920 1080
sudo perf stat -a -r 5 -e "power/energy-pkg/","power/energy-cores/","power/energy-ram/" mpiexec --oversubscribe -n 4 --allow-run-as-root  ./main_par 3840 2176
sudo perf stat -a -r 5 -e "power/energy-pkg/","power/energy-cores/","power/energy-ram/" mpiexec --oversubscribe -n 4 --allow-run-as-root  ./main_par 7680 4352

sudo perf stat -a -r 5 -e "power/energy-pkg/","power/energy-cores/","power/energy-ram/" mpiexec --oversubscribe -n 8 --allow-run-as-root  ./main_par 1920 1080
sudo perf stat -a -r 5 -e "power/energy-pkg/","power/energy-cores/","power/energy-ram/" mpiexec --oversubscribe -n 8 --allow-run-as-root  ./main_par 3840 2176
sudo perf stat -a -r 5 -e "power/energy-pkg/","power/energy-cores/","power/energy-ram/" mpiexec --oversubscribe -n 8 --allow-run-as-root  ./main_par 7680 4352

sudo perf stat -a -r 5 -e "power/energy-pkg/","power/energy-cores/","power/energy-ram/" mpiexec --oversubscribe -n 16 --allow-run-as-root  ./main_par 1920 1088
sudo perf stat -a -r 5 -e "power/energy-pkg/","power/energy-cores/","power/energy-ram/" mpiexec --oversubscribe -n 16 --allow-run-as-root  ./main_par 3840 2176
sudo perf stat -a -r 5 -e "power/energy-pkg/","power/energy-cores/","power/energy-ram/" mpiexec --oversubscribe -n 16 --allow-run-as-root  ./main_par 7680 4352
