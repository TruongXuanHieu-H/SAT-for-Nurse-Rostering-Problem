import matplotlib.pyplot as plt
import numpy as np

x = np.array([40, 60, 80, 100])

y_bdd = np.array([49, 94, 145, 187])
y_card = np.array([107, 143, 180, 212])
y_pairwise = np.array([np.nan, np.nan, np.nan, np.nan])  # MO
y_scl = np.array([16, 25, 33, 42])
y_seq = np.array([27, 53, 86, 106])

# Các solver thêm (không thuộc mapping cố định nên dùng màu trung tính)
y_cplex = np.array([38, 57, 81, 82])
y_cpsat = np.array([59, 83, 140, 148])
y_gurobi = np.array([18, 27, 41, 59])

plt.figure(figsize=(10,6))

# Theo đúng quy ước màu cố định của bạn
plt.plot(x, y_pairwise, linestyle=':',  color='blue',   label="Pairwise", linewidth=2)
plt.plot(x, y_seq,      linestyle='--', color='orange', label="Seq", linewidth=2)
plt.plot(x, y_bdd,      linestyle='-.', color='purple', label="BDD", linewidth=2)
plt.plot(x, y_card,     linestyle=':',  color='green',  label="Card", linewidth=2)
plt.plot(x, y_scl,      linestyle='-',  color='black',  label="SCL", linewidth=2)

# Solver khác (không dùng red/magenta vì reserved)
plt.plot(x, y_cplex,    linestyle='--', color='brown',  label="CPLEX", linewidth=2)
plt.plot(x, y_cpsat,    linestyle='-.', color='gray',   label="CP-SAT", linewidth=2)
plt.plot(x, y_gurobi,   linestyle='-',  color='olive',  label="Gurobi", linewidth=2)

plt.xlabel("Work Period (days)")
plt.ylabel("Time (ms)")
plt.title("NRP Solving Time Comparison")
plt.xticks(x)
plt.grid(True)
plt.legend()
plt.tight_layout()

plt.show()