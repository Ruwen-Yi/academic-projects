#!/usr/bin/env bash

#SBATCH -N 1
#SBATCH --time=01:00:00
#SBATCH --mem=100MB

echo "sum,matrix_size,num_threads,timer"

for ((x=0; x<5; x++)); do
    for ((y=0; y<=32; y++)); do
    ./diagonal 40000 $y
    done
done
