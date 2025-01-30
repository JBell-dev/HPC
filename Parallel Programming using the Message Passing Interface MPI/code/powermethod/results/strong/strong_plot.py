import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

df = pd.read_csv('strong_scaling.csv')


df['ideal_time'] = df['runtime'].iloc[0] / df['processes'] #i consider the ideal as perfect scaling of the single process time
df['efficiency'] = (df['runtime'].iloc[0] / (df['processes'] * df['runtime'])) * 100
# (T1 / (p * Tp)) * 100
# where T1 is single process time, p is number of processes, Tp is time with p processes
# https://scicomp.ethz.ch/wiki/Parallel_efficiency

fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(15, 6))

# Runtime vs Processes 
ax1.plot(df['processes'], df['runtime'], 'bo-', label='Actual')
ax1.plot(df['processes'], df['ideal_time'], 'r--', label='Ideal')
ax1.set_xlabel('Number of Processes')
ax1.set_ylabel('Runtime (seconds)')
ax1.set_title('Strong Scaling Performance')
ax1.grid(True)
ax1.legend()
ax1.set_yscale('log', base=2)
ax1.set_xscale('log', base=2)

#Parallel Efficiency
ax2.plot(df['processes'], df['efficiency'], 'go-')
ax2.set_xlabel('Number of Processes')
ax2.set_ylabel('Parallel Efficiency (%)')
ax2.set_title('Parallel Efficiency')
ax2.grid(True)
ax2.set_xscale('log', base=2)


plt.tight_layout()
plt.savefig('strong_scaling.png')

# Print analysis
print("\nStrong Scaling Analysis:")
print("-" * 44)
print(f"{'Processes':<10} {'Runtime(s)':<10} {'Speedup':<10} {'Efficiency(%)':<10}")
print("-" * 44)
for _, row in df.iterrows():
    speedup = df['runtime'].iloc[0] / row['runtime']
    print(f"{int(row['processes']):<10} {row['runtime']:<10f} {speedup:<10f} {row['efficiency']:<10f}")