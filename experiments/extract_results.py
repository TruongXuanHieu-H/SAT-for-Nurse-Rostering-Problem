import os
import re
import argparse
import pandas as pd

# ===== regex nội dung log =====
RE_SAT = re.compile(r"RESULT\s+============================")
RE_KILL = re.compile(r"Limit violated")
RE_TIME = re.compile(r"Wall clock time:\s*([\d.]+)\s*s")
RE_MEM = re.compile(r"Peak memory:\s*([\d.]+)\s*MB")
RE_CLAUSE = re.compile(r"Total clauses used:\s*(\d+)")
RE_VAR = re.compile(r"Total variables used:\s*(\d+)")

# ===== regex tên file =====
RE_FILENAME = re.compile(
    r"nrp_(\d+)_(\d+)_([a-zA-Z0-9]+)_(\d+)\.txt"
)

def main(log_dir, output_excel):
    rows = []

    # ---------- đọc log ----------
    for root, _, files in os.walk(log_dir):
        for fname in files:
            if not fname.endswith(".txt"):
                continue

            m = RE_FILENAME.match(fname)
            if not m:
                continue

            number_nurse, work_period, encoding, run = m.groups()
            path = os.path.join(root, fname)

            with open(path, "r", encoding="utf-8", errors="ignore") as f:
                content = f.read()

            solved = bool(RE_SAT.search(content))
            killed = bool(RE_KILL.search(content))

            m_time = RE_TIME.search(content)
            wall_time = float(m_time.group(1)) if m_time else None

            m_mem = RE_MEM.search(content)
            peak_mem = float(m_mem.group(1)) if m_mem else None

            if solved:
                m_clause = RE_CLAUSE.search(content)
                total_clauses = int(m_clause.group(1)) if m_clause else None

                m_var = RE_VAR.search(content)
                total_vars = int(m_var.group(1)) if m_var else None
            else:
                total_clauses = None
                total_vars = None

            rows.append({
                "number_nurse": int(number_nurse),
                "work_period": int(work_period),
                "encoding": encoding,
                "run": int(run),
                "solved": solved,
                "wall_time_sec": wall_time,
                "peak_memory_mb": peak_mem,
                "total_clauses": total_clauses,
                "total_variables": total_vars
            })

    df = pd.DataFrame(rows)

    df = df.sort_values(
        by=["number_nurse", "work_period", "encoding", "run"]
    )

    # ---------- AVG cho mọi encoding ----------
    avg_rows = []

    for (n, w, e), g in df.groupby(
        ["number_nurse", "work_period", "encoding"]
    ):
        solved_runs = g[g["solved"] == True]

        avg_rows.append({
            "number_nurse": n,
            "work_period": w,
            "encoding": e,
            "run": "AVG",
            "solved": solved_runs.shape[0] > 0,
            "wall_time_sec": g["wall_time_sec"].mean(),
            "peak_memory_mb": g["peak_memory_mb"].mean(),
            "total_clauses": solved_runs["total_clauses"].mean()
                if not solved_runs.empty else None,
            "total_variables": solved_runs["total_variables"].mean()
                if not solved_runs.empty else None,
        })

    avg_df = pd.DataFrame(avg_rows)

    final_df = pd.concat([df, avg_df], ignore_index=True)

    final_df = final_df.sort_values(
        by=["number_nurse", "work_period", "encoding", "run"],
        key=lambda x: x.astype(str)
    )

    final_df.to_excel(output_excel, index=False)

    print(f"✓ Runs extracted: {len(df)}")
    print(f"✓ AVG rows added: {len(avg_df)}")
    print(f"✓ Output written to: {output_excel}")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description="NRP benchmark extractor with AVG for all encodings"
    )
    parser.add_argument("log_dir", help="Root results directory")
    parser.add_argument("output", help="Output Excel file (.xlsx)")

    args = parser.parse_args()
    main(args.log_dir, args.output)
