import os
import re
from collections import defaultdict
import pandas as pd

# =========================
# CONFIG
# =========================
LOG_DIR = "cplex_log"
OUTPUT_XLSX = "nrp_cplex_exp_results.xlsx"

# =========================
# REGEX
# =========================
FILENAME_RE = re.compile(r"NRP_(\d+)_(\d+)_run(\d+)\.out")
MEMORY_RE = re.compile(r"Total memory usage\s*:\s*([0-9.]+)\s*MB")
TIME_RE = re.compile(r"Time spent in solve\s*:\s*([0-9.]+)s")

# =========================
# EXTRACT
# =========================
results = []

for fname in os.listdir(LOG_DIR):
    m = FILENAME_RE.match(fname)
    if not m:
        continue

    nurses, days, run = map(int, m.groups())
    path = os.path.join(LOG_DIR, fname)

    with open(path, "r", encoding="utf-8", errors="ignore") as f:
        content = f.read()

    mem_match = MEMORY_RE.search(content)
    time_match = TIME_RE.search(content)

    results.append({
        "file": fname,
        "nurses": nurses,
        "days": days,
        "run": run,
        "total_memory_mb": float(mem_match.group(1)) if mem_match else None,
        "solve_time_s": float(time_match.group(1)) if time_match else None
    })

# =========================
# SORT
# =========================
results.sort(key=lambda r: (r["nurses"], r["days"], r["run"]))

# =========================
# GROUP BY INSTANCE
# =========================
groups = defaultdict(list)
for r in results:
    groups[(r["nurses"], r["days"])].append(r)

# =========================
# WRITE EXCEL (MULTI-SHEET)
# =========================
with pd.ExcelWriter(OUTPUT_XLSX, engine="openpyxl") as writer:
    for (nurses, days), rows in sorted(groups.items()):
        df = pd.DataFrame(rows).sort_values(by="run")

        # Compute average row
        avg_row = {
            "file": "AVERAGE",
            "nurses": nurses,
            "days": days,
            "run": "",
            "total_memory_mb": df["total_memory_mb"].mean(),
            "solve_time_s": df["solve_time_s"].mean()
        }

        df = pd.concat([df, pd.DataFrame([avg_row])], ignore_index=True)

        sheet_name = f"N{nurses}_D{days}"
        df.to_excel(writer, sheet_name=sheet_name, index=False)

print(f"Done: {len(groups)} instances written to {OUTPUT_XLSX}")
