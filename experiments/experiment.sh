#!/bin/bash

LIMIT_MEMORY=5000
LIMIT_REAL_TIME=600

NURSES=(1)
ENCODINGS=(pairwise)
DAYS=(40 60 80 100)
RUNS=10

for enc in "${ENCODINGS[@]}"; do
  mkdir -p "results/$enc"
done

for enc in "${ENCODINGS[@]}"; do
  for n in "${NURSES[@]}"; do
    for d in "${DAYS[@]}"; do
      for RUN in $(seq 1 $RUNS); do
        ../build/nrp_enc $n $d --$enc -limit-memory $LIMIT_MEMORY  -limit-real-time $LIMIT_REAL_TIME  2>&1 | tee results/$enc/nrp_${n}_${d}_${enc}_${RUN}.txt
      done
    done
  done
done
