import sys
import gurobipy as gp
from gurobipy import GRB
import time
import psutil
import os

# Parameters
number_nurses = int(sys.argv[1]) # Number of nurses
number_days = int(sys.argv[2]) # Number of days

# Hard configurations
number_shifts_per_day = 3 # 0: day, 1: evening, 2: night
off_day_index = number_shifts_per_day # 3: off day

# Data
NURSES = range(number_nurses)         
DAYS = range(number_days)     
WORK_SHIFTS = range(number_shifts_per_day)
SHIFTS = range(number_shifts_per_day + 1)

process = psutil.Process(os.getpid())
mem_before = process.memory_info().rss / (1024 * 1024)
start_processing_time = time.time()

model = gp.Model("Nurse Scheduling")
x = model.addVars(NURSES, DAYS, SHIFTS, vtype=GRB.BINARY, name="x")

# Objective: Max 1 shift per nurse per day
for n in NURSES:
    for d in DAYS:
        model.addConstr(gp.quicksum(x[n, d, s] for s in SHIFTS) == 1)

# Objective: Min 20 shifts per nurse per 28 days
for n in NURSES:
    for start in range(number_days - 27):
        model.addConstr(
            gp.quicksum(x[n, d, s] for d in range(start, start+28) for s in WORK_SHIFTS) >= 20
        )

# Objective: Min 4 off days per nurse per 14 days
for n in NURSES:
    for start in range(number_days - 13):
        model.addConstr(
            gp.quicksum(x[n, d, 3] for d in range(start, start+14)) >= 4
        )

# Objective: Between 1 and 4 night shift every 14 days
for n in NURSES:
    for start in range(number_days - 13):
        night_shifts = gp.quicksum(x[n, d, 2] for d in range(start, start+14))
        model.addConstr(night_shifts >= 1)
        model.addConstr(night_shifts <= 4)

# Objective: Betweeen 4 and 8 evening shifts per nurse per 14 days
for n in NURSES:
    for start in range(number_days - 13):
        evening_shifts = gp.quicksum(x[n, d, 1] for d in range(start, start+14))
        model.addConstr(evening_shifts >= 4)
        model.addConstr(evening_shifts <= 8)

# Objective: No consecutive night shifts
for n in NURSES:
    for d in range(number_days - 1): 
        model.addConstr(x[n, d, 2] + x[n, d+1, 2] <= 1)

# Objective: Between 2 and 4 evening/night shifts every 7 days
for n in NURSES:
    for start in range(number_days - 6):
        evening_night_shifts = gp.quicksum(x[n, d, s] for d in range(start, start+7) for s in {1, 2})
        model.addConstr(evening_night_shifts >= 2)
        model.addConstr(evening_night_shifts <= 4)

# Objective: Max 6 shifts per nurse per 7 days
for n in NURSES:
    for start in range(number_days - 6):
        model.addConstr(
            gp.quicksum(x[n, d, s] for d in range(start, start+7) for s in WORK_SHIFTS) <= 6
        )

end_processing_time = time.time()

print("------------------Gurobi Log------------------")
start_optimizing_time = time.time()
model.optimize()

end_optimizing_time = time.time()
mem_after = process.memory_info().rss / (1024 * 1024)


def print_specifications():
    print("------------------Specifications------------------")
    processing_time = end_processing_time - start_processing_time
    optimization_time = end_optimizing_time - start_optimizing_time
    print(f"Processing Time     : {processing_time:.3f} seconds")
    print(f"Optimization Time   : {optimization_time:.3f} seconds")
    print(f"Total Time          : {processing_time + optimization_time:.3f} seconds")
    print(f"Memory Usaged       : {mem_after - mem_before:.2f} MB")

def verify_solution(x, NURSES, DAYS, WORK_SHIFTS):
    print("------------------Verifying solution------------------")
    errors = 0
    number_days = len(DAYS)

    for n in NURSES:
        # C1: Max 1 shift per nurse per day
        for d in DAYS:
            total = sum(x[n, d, s].X for s in SHIFTS)
            if (total > 1 + 1e-6 or total < 1 - 1e-6) :
                print(f"C1 Violation: Nurse {n}, Day {d}, shifts assigned = {total}")
                errors += 1

        # C5: At least 20 total shifts in 28 days
        for start in range(number_days - 27):
            total = sum(x[n, d, s].X for d in range(start, start + 28) for s in WORK_SHIFTS)
            if total < 20 - 1e-6:
                print(f"C5 Violation: Nurse {n}, Days {start}-{start+27}, shifts = {total}")
                errors += 1

        # C3: Min 4 off-days in any 14 consecutive days
        for start in range(number_days - 13):
            total = sum(x[n, d, 3].X for d in range(start, start + 14))
            if (total < 4 - 1e-6):
                print(f"C3 Violation: Nurse {n}, Days {start}-{start+13}, shifts = {total}")
                errors += 1

        # C6: Between 1 and 4 night shifts (shift 2) in 14 days
        for start in range(number_days - 13):
            night = sum(x[n, d, 2].X for d in range(start, start + 14))
            if not (1 - 1e-6 <= night <= 4 + 1e-6):
                print(f"C6 Violation: Nurse {n}, Days {start}-{start+13}, night shifts = {night}")
                errors += 1

        # C4: Between 4 and 8 evening shifts (shift 1) in 14 days
        for start in range(number_days - 13):
            evening = sum(x[n, d, 1].X for d in range(start, start + 14))
            if not (4 - 1e-6 <= evening <= 8 + 1e-6):
                print(f"C4 Violation: Nurse {n}, Days {start}-{start+13}, evening shifts = {evening}")
                errors += 1

        # C8: No consecutive night shifts
        for d in range(number_days - 1):
            total = x[n, d, 2].X + x[n, d + 1, 2].X
            if total > 1 + 1e-6:
                print(f"C8 Violation: Nurse {n}, consecutive night shifts on Days {d}-{d+1}")
                errors += 1

        # C7: Between 2 and 4 evening or night shifts in 7 days
        for start in range(number_days - 6):
            evening = sum(x[n, d, s].X for d in range(start, start + 7) for s in {1, 2})
            if not (2 - 1e-6 <= evening <= 4 + 1e-6):
                print(f"C7 Violation: Nurse {n}, Days {start}-{start+6}, evening shifts = {evening}")
                errors += 1

        # C2: Max 6 shifts in any 7 consecutive days
        for start in range(number_days - 6):
            total = sum(x[n, d, s].X for d in range(start, start + 7) for s in WORK_SHIFTS)
            if total > 6 + 1e-6:
                print(f"C2 Violation: Nurse {n}, Days {start}-{start+6}, shifts = {total}")
                errors += 1

    if errors == 0:
        print("All constraints are satisfied.")
    else:
        print(f"Total violations found: {errors}")

def print_solution(x, NURSES, DAYS, WORK_SHIFTS):
    print("------------------Schedule------------------")
    for n in NURSES:
        schedule = f"Nurse {n}: "
        for d in DAYS:
            shift = off_day_index
            for s in WORK_SHIFTS:
                if x[n, d, s].X > 0.5:
                    if shift == off_day_index:
                        shift = s
                    else:
                        raise ValueError("Duplicate shifts found for a nurse on a day.")
            schedule += shift.__str__() + " "
        print(schedule.strip())

print_specifications()

if model.status == GRB.INFEASIBLE:
    print("Model is infeasible.")
else:
    verify_solution(x, NURSES, DAYS, WORK_SHIFTS)
    print_solution(x, NURSES, DAYS, WORK_SHIFTS)

