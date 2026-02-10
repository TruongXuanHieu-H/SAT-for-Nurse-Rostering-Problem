import sys
import time
import psutil
import os
from ortools.sat.python import cp_model

# ===============================
# Resource tracking
# ===============================
process = psutil.Process(os.getpid())
mem_before = process.memory_info().rss / (1024 * 1024)
start_processing_time = time.time()

# ===============================
# Parameters
# ===============================
number_nurses = int(sys.argv[1])
number_days = int(sys.argv[2])

number_shifts_per_day = 3   # 0: Day, 1: Evening, 2: Night
off_day_index = 3           # Off

NURSES = range(number_nurses)
DAYS = range(number_days)
WORK_SHIFTS = range(3)
SHIFTS = range(4)

# ===============================
# Model
# ===============================
model = cp_model.CpModel()

# Decision variables
x = {}
for n in NURSES:
    for d in DAYS:
        for s in SHIFTS:
            x[n, d, s] = model.NewBoolVar(f"x_{n}_{d}_{s}")

# ===============================
# C1: Exactly one shift per nurse per day
# ===============================
for n in NURSES:
    for d in DAYS:
        model.Add(sum(x[n, d, s] for s in SHIFTS) == 1)

# ===============================
# C5: Min 20 working shifts in any 28 days
# ===============================
for n in NURSES:
    for start in range(number_days - 27):
        model.Add(
            sum(x[n, d, s]
                for d in range(start, start + 28)
                for s in WORK_SHIFTS) >= 20
        )

# ===============================
# C3: Min 4 off-days in any 14 days
# ===============================
for n in NURSES:
    for start in range(number_days - 13):
        model.Add(
            sum(x[n, d, off_day_index]
                for d in range(start, start + 14)) >= 4
        )

# ===============================
# C6: 1–4 night shifts in any 14 days
# ===============================
for n in NURSES:
    for start in range(number_days - 13):
        night = sum(x[n, d, 2] for d in range(start, start + 14))
        model.Add(night >= 1)
        model.Add(night <= 4)

# ===============================
# C4: 4–8 evening shifts in any 14 days
# ===============================
for n in NURSES:
    for start in range(number_days - 13):
        evening = sum(x[n, d, 1] for d in range(start, start + 14))
        model.Add(evening >= 4)
        model.Add(evening <= 8)

# ===============================
# C8: No consecutive night shifts
# ===============================
for n in NURSES:
    for d in range(number_days - 1):
        model.Add(x[n, d, 2] + x[n, d + 1, 2] <= 1)

# ===============================
# C7: 2–4 evening/night shifts in any 7 days
# ===============================
for n in NURSES:
    for start in range(number_days - 6):
        en = sum(x[n, d, s]
                 for d in range(start, start + 7)
                 for s in [1, 2])
        model.Add(en >= 2)
        model.Add(en <= 4)

# ===============================
# C2: Max 6 working shifts in any 7 days
# ===============================
for n in NURSES:
    for start in range(number_days - 6):
        model.Add(
            sum(x[n, d, s]
                for d in range(start, start + 7)
                for s in WORK_SHIFTS) <= 6
        )

end_processing_time = time.time()

# ===============================
# Solve
# ===============================
solver = cp_model.CpSolver()
solver.parameters.num_search_workers = 8  # dùng đa luồng
start_optimizing_time = time.time()
status = solver.Solve(model)
end_optimizing_time = time.time()

mem_after = process.memory_info().rss / (1024 * 1024)

# ===============================
# Output
# ===============================
print("------------------CP-SAT Stats------------------")
print(f"Status             : {solver.StatusName(status)}")
print(f"Processing Time    : {end_processing_time - start_processing_time:.3f}s")
print(f"Solving Time       : {end_optimizing_time - start_optimizing_time:.3f}s")
print(f"Total Time         : {(end_optimizing_time - start_processing_time):.3f}s")
print(f"Memory Used        : {mem_after - mem_before:.2f} MB")

def verify_solution_cp_sat(x, solver, NURSES, DAYS, WORK_SHIFTS, SHIFTS):
    print("------------------Verifying solution------------------")
    errors = 0
    number_days = len(DAYS)

    for n in NURSES:
        # C1: Exactly 1 shift per nurse per day
        for d in DAYS:
            total = sum(solver.Value(x[n, d, s]) for s in SHIFTS)
            if total != 1:
                print(f"C1 Violation: Nurse {n}, Day {d}, assigned = {total}")
                errors += 1

        # C5: Min 20 working shifts in any 28 days
        for start in range(number_days - 27):
            total = sum(
                solver.Value(x[n, d, s])
                for d in range(start, start + 28)
                for s in WORK_SHIFTS
            )
            if total < 20:
                print(f"C5 Violation: Nurse {n}, Days {start}-{start+27}, shifts = {total}")
                errors += 1

        # C3: Min 4 off-days in any 14 days
        for start in range(number_days - 13):
            off = sum(
                solver.Value(x[n, d, 3])
                for d in range(start, start + 14)
            )
            if off < 4:
                print(f"C3 Violation: Nurse {n}, Days {start}-{start+13}, off-days = {off}")
                errors += 1

        # C6: 1–4 night shifts in any 14 days
        for start in range(number_days - 13):
            night = sum(
                solver.Value(x[n, d, 2])
                for d in range(start, start + 14)
            )
            if night < 1 or night > 4:
                print(f"C6 Violation: Nurse {n}, Days {start}-{start+13}, night = {night}")
                errors += 1

        # C4: 4–8 evening shifts in any 14 days
        for start in range(number_days - 13):
            evening = sum(
                solver.Value(x[n, d, 1])
                for d in range(start, start + 14)
            )
            if evening < 4 or evening > 8:
                print(f"C4 Violation: Nurse {n}, Days {start}-{start+13}, evening = {evening}")
                errors += 1

        # C8: No consecutive night shifts
        for d in range(number_days - 1):
            if solver.Value(x[n, d, 2]) + solver.Value(x[n, d + 1, 2]) > 1:
                print(f"C8 Violation: Nurse {n}, consecutive nights on Days {d}-{d+1}")
                errors += 1

        # C7: 2–4 evening/night shifts in any 7 days
        for start in range(number_days - 6):
            en = sum(
                solver.Value(x[n, d, s])
                for d in range(start, start + 7)
                for s in [1, 2]
            )
            if en < 2 or en > 4:
                print(f"C7 Violation: Nurse {n}, Days {start}-{start+6}, E+N = {en}")
                errors += 1

        # C2: Max 6 working shifts in any 7 days
        for start in range(number_days - 6):
            total = sum(
                solver.Value(x[n, d, s])
                for d in range(start, start + 7)
                for s in WORK_SHIFTS
            )
            if total > 6:
                print(f"C2 Violation: Nurse {n}, Days {start}-{start+6}, shifts = {total}")
                errors += 1

    if errors == 0:
        print("All constraints are satisfied.")
    else:
        print(f"Total violations found: {errors}")


# ===============================
# Print solution
# ===============================
if status in (cp_model.OPTIMAL, cp_model.FEASIBLE):
    verify_solution_cp_sat(x, solver, NURSES, DAYS, WORK_SHIFTS, SHIFTS)
    print("------------------Schedule------------------")
    for n in NURSES:
        row = f"Nurse {n}: "
        for d in DAYS:
            for s in SHIFTS:
                if solver.Value(x[n, d, s]) == 1:
                    row += str(s) + " "
                    break
        print(row.strip())
else:
    print("No feasible solution found.")
