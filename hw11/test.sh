#!/bin/bash

echo "Calculate average: "

cat $1 | grep "Execution Time:" | gawk '{print $2}' FS=": " | awk -F : '{sum+=$1} END {printf("AVG = %.8f\n", sum/NR)}'
