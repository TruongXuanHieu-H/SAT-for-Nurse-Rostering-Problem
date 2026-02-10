#!/bin/bash

chmod +x runlim

for d in 40 60 80 100; do
  for i in $(seq 1 10); do
    oplrun model/NRP.mod data/NRP_1_${d}.input 2>&1 | tee cplex_log/NRP_1_${d}_run${i}.out
  done
done

