#!/usr/bin/env bash

#SBATCH -N 1
#SBATCH --time=01:00:00
#SBATCH --mem=100MB

echo "sum,matrix_size,num_threads,timer"

for x in 100 200 300; do
    ./diagonal $x 100
done
