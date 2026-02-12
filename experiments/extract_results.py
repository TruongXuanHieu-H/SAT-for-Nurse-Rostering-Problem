import os
import re
import argparse
import pandas as pd

RE_SAT = re.compile(r"RESULT\s+============================")
RE_KILL = re.compile(r"Limit violated")
RE_ENCODING_TIME = re.compile(r"Encoding time:\s*([\d.]+)\s*s")
RE_SOLVING_TIME = re.compile(r"Solving time:\s*([\d.]+)\s*s")
RE_TOTAL_TIME = re.compile(r"Total time:\s*([\d.]+)\s*s")
RE_MEM = re.compile(r"Peak memory:\s*([\d.]+)\s*MB")
RE_CLAUSE = re.compile(r"Total clauses used:\s*(\d+)")
RE_VAR = re.compile(r"Total variables used:\s*(\d+)")

RE_FILENAME = re.compile(
    r"nrp_(\d+)_(\d+)_([a-zA-Z0-9]+)_(\d+)\.txt"
)

def main(log_dir, output_dir):
    rows = []

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

            m_enc = RE_ENCODING_TIME.search(content)
            encoding_time = float(m_enc.group(1)) if m_enc else None

            m_sol = RE_SOLVING_TIME.search(content)
            solving_time = float(m_sol.group(1)) if m_sol else None

            m_tot = RE_TOTAL_TIME.search(content)
            total_time = float(m_tot.group(1)) if m_tot else None

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
                "encoding_time_sec": encoding_time,
                "solving_time_sec": solving_time,
                "total_time_sec": total_time,
                "peak_memory_mb": peak_mem,
                "total_clauses": total_clauses,
                "total_variables": total_vars
            })

    df = pd.DataFrame(rows)

    df = df.sort_values(
        by=["number_nurse", "work_period", "encoding", "run"]
    )

    # ---------- AVG ----------
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
            "encoding_time_sec": g["encoding_time_sec"].mean(),
            "solving_time_sec": g["solving_time_sec"].mean(),
            "total_time_sec": g["total_time_sec"].mean(),
            "peak_memory_mb": g["peak_memory_mb"].mean(),
            "total_clauses": solved_runs["total_clauses"].mean()
                if not solved_runs.empty else None,
            "total_variables": solved_runs["total_variables"].mean()
                if not solved_runs.empty else None,
        })

    avg_df = pd.DataFrame(avg_rows)
    final_df = pd.concat([df, avg_df], ignore_index=True)

    os.makedirs(output_dir, exist_ok=True)

    for encoding, enc_df in final_df.groupby("encoding"):
        out_file = os.path.join(
            output_dir,
            f"nrp_sat_{encoding}_exp_results.xlsx"
        )

        with pd.ExcelWriter(out_file, engine="openpyxl") as writer:
            for (n, w), g in enc_df.groupby(
                ["number_nurse", "work_period"]
            ):
                sheet_name = f"n{n}_d{w}"

                g = g.copy()
                g["__run_order"] = g["run"].apply(
                    lambda x: x if isinstance(x, int) else 10**9
                )
                g = g.sort_values("__run_order").drop(
                    columns="__run_order"
                )

                g.to_excel(
                    writer,
                    sheet_name=sheet_name,
                    index=False
                )

        print(f"[OK] Written: {out_file}")

if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description="NRP benchmark extractor (per encoding, per instance sheets)"
    )
    parser.add_argument("log_dir", help="Root results directory")
    parser.add_argument(
        "-o", "--output_dir",
        default=".",
        help="Output directory for Excel files"
    )

    args = parser.parse_args()
    main(args.log_dir, args.output_dir)
