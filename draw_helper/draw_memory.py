import matplotlib.pyplot as plt
import numpy as np

x = np.array([40, 60, 80, 100])

# Memory (MB)
y_bdd = np.array([8.9, 15.2, 23, 28])
y_card = np.array([10.8, 16.7, 24.5, 29.5])
y_pairwise = np.array([np.nan, np.nan, np.nan, np.nan])  # MO
y_scl = np.array([5.7, 8.6, 11.5, 13.4])
y_seq = np.array([13.9, 26, 40.2, 47.4])
y_cplex = np.array([9.3, 12.5, 16, 19.4])
y_cpsat = np.array([17.1, 19.1, 26, 29.9])
y_gurobi = np.array([15.4, 14.8, 18.4, 16.4])

plt.figure(figsize=(10,6))

# Encoding cố định
plt.plot(x, y_pairwise, linestyle=':',  color='blue',   label="Pairwise", linewidth=2)
plt.plot(x, y_seq,      linestyle='--', color='orange', label="Seq", linewidth=2)
plt.plot(x, y_bdd,      linestyle='-.', color='purple', label="BDD", linewidth=2)
plt.plot(x, y_card,     linestyle=':',  color='green',  label="Card", linewidth=2)
plt.plot(x, y_scl,      linestyle='-',  color='black',  label="SCL", linewidth=2)

# Solver khác (không dùng red/magenta)
plt.plot(x, y_cplex,    linestyle='--', color='brown', label="CPLEX", linewidth=2)
plt.plot(x, y_cpsat,    linestyle='-.', color='gray',  label="CP-SAT", linewidth=2)
plt.plot(x, y_gurobi,   linestyle='-',  color='olive', label="Gurobi", linewidth=2)

plt.xlabel("Work Period (days)")
plt.ylabel("Memory (MB)")
plt.title("NRP Memory Consumption Comparison")
plt.xticks(x)
plt.grid(True)
plt.legend()
plt.tight_layout()

plt.show()