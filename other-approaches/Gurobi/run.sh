#!/bin/bash

for d in 40 60 80 100; do
  for i in $(seq 1 10); do
    python3 NRP_gurobi.py 1 $d 2>&1 | tee gurobi_log/NRP_1_${d}_run${i}.out
  done
done
