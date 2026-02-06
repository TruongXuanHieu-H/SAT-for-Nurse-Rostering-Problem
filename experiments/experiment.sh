#!/bin/bash

LIMIT_MEMORY=5000
LIMIT_REAL_TIME=600

mkdir -p results/{pairwise,bdd,card,scl,seq}

for RUN in 1 2 3 4 5 6 7 8 9 10; do

  # ===== pairwise =====
  ../build/nrp_enc 1 40  --pairwise -limit-memory $LIMIT_MEMORY -limit-real-time $LIMIT_REAL_TIME 2>&1 | tee results/pairwise/nrp_1_40_pairwise_$RUN.txt
  ../build/nrp_enc 1 60  --pairwise -limit-memory $LIMIT_MEMORY -limit-real-time $LIMIT_REAL_TIME 2>&1 | tee results/pairwise/nrp_1_60_pairwise_$RUN.txt
  ../build/nrp_enc 1 80  --pairwise -limit-memory $LIMIT_MEMORY -limit-real-time $LIMIT_REAL_TIME 2>&1 | tee results/pairwise/nrp_1_80_pairwise_$RUN.txt
  ../build/nrp_enc 1 100 --pairwise -limit-memory $LIMIT_MEMORY -limit-real-time $LIMIT_REAL_TIME 2>&1 | tee results/pairwise/nrp_1_100_pairwise_$RUN.txt

  # ===== bdd =====
  ../build/nrp_enc 1 40  --bdd -limit-memory $LIMIT_MEMORY -limit-real-time $LIMIT_REAL_TIME 2>&1 | tee results/bdd/nrp_1_40_bdd_$RUN.txt
  ../build/nrp_enc 1 60  --bdd -limit-memory $LIMIT_MEMORY -limit-real-time $LIMIT_REAL_TIME 2>&1 | tee results/bdd/nrp_1_60_bdd_$RUN.txt
  ../build/nrp_enc 1 80  --bdd -limit-memory $LIMIT_MEMORY -limit-real-time $LIMIT_REAL_TIME 2>&1 | tee results/bdd/nrp_1_80_bdd_$RUN.txt
  ../build/nrp_enc 1 100 --bdd -limit-memory $LIMIT_MEMORY -limit-real-time $LIMIT_REAL_TIME 2>&1 | tee results/bdd/nrp_1_100_bdd_$RUN.txt

  # ===== card =====
  ../build/nrp_enc 1 40  --card -limit-memory $LIMIT_MEMORY -limit-real-time $LIMIT_REAL_TIME 2>&1 | tee results/card/nrp_1_40_card_$RUN.txt
  ../build/nrp_enc 1 60  --card -limit-memory $LIMIT_MEMORY -limit-real-time $LIMIT_REAL_TIME 2>&1 | tee results/card/nrp_1_60_card_$RUN.txt
  ../build/nrp_enc 1 80  --card -limit-memory $LIMIT_MEMORY -limit-real-time $LIMIT_REAL_TIME 2>&1 | tee results/card/nrp_1_80_card_$RUN.txt
  ../build/nrp_enc 1 100 --card -limit-memory $LIMIT_MEMORY -limit-real-time $LIMIT_REAL_TIME 2>&1 | tee results/card/nrp_1_100_card_$RUN.txt

  # ===== scl =====
  ../build/nrp_enc 1 40  --scl -limit-memory $LIMIT_MEMORY -limit-real-time $LIMIT_REAL_TIME 2>&1 | tee results/scl/nrp_1_40_scl_$RUN.txt
  ../build/nrp_enc 1 60  --scl -limit-memory $LIMIT_MEMORY -limit-real-time $LIMIT_REAL_TIME 2>&1 | tee results/scl/nrp_1_60_scl_$RUN.txt
  ../build/nrp_enc 1 80  --scl -limit-memory $LIMIT_MEMORY -limit-real-time $LIMIT_REAL_TIME 2>&1 | tee results/scl/nrp_1_80_scl_$RUN.txt
  ../build/nrp_enc 1 100 --scl -limit-memory $LIMIT_MEMORY -limit-real-time $LIMIT_REAL_TIME 2>&1 | tee results/scl/nrp_1_100_scl_$RUN.txt

  # ===== seq =====
  ../build/nrp_enc 1 40  --seq -limit-memory $LIMIT_MEMORY -limit-real-time $LIMIT_REAL_TIME 2>&1 | tee results/seq/nrp_1_40_seq_$RUN.txt
  ../build/nrp_enc 1 60  --seq -limit-memory $LIMIT_MEMORY -limit-real-time $LIMIT_REAL_TIME 2>&1 | tee results/seq/nrp_1_60_seq_$RUN.txt
  ../build/nrp_enc 1 80  --seq -limit-memory $LIMIT_MEMORY -limit-real-time $LIMIT_REAL_TIME 2>&1 | tee results/seq/nrp_1_80_seq_$RUN.txt
  ../build/nrp_enc 1 100 --seq -limit-memory $LIMIT_MEMORY -limit-real-time $LIMIT_REAL_TIME 2>&1 | tee results/seq/nrp_1_100_seq_$RUN.txt

done
