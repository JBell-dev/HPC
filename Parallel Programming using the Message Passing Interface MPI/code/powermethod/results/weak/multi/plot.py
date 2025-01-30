import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

df = pd.read_csv('weak_scaling.csv')

# For weak scaling, ideal runtime should stay constant
df['ideal_time'] = df['runtime'].iloc[0]  # keep constant
df['efficiency'] = (df['runtime'].iloc[0] / df['runtime']) * 100
# (T1/Tp) since weak scaling keeps constant the problem size


fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(15, 6))

# Runtime vs Processes 
ax1.plot(df['processes'], df['runtime'], 'bo-', label='Actual')
ax1.plot(df['processes'], df['ideal_time'], 'r--', label='Ideal')
ax1.set_xlabel('Number of Processes')
ax1.set_ylabel('Runtime (seconds)')
ax1.set_title('Weak Scaling Performance')
ax1.grid(True)
ax1.legend()
ax1.set_xscale('log', base=2)

# Parallel Efficiency
ax2.plot(df['processes'], df['efficiency'], 'go-')
ax2.set_xlabel('Number of Processes')
ax2.set_ylabel('Parallel Efficiency (%)')
ax2.set_title('Parallel Efficiency')
ax2.grid(True)
ax2.set_xscale('log', base=2)

plt.tight_layout()
plt.savefig('weak_scaling.png')
print("\nWeak Scaling Analysis:")
print("-" * 54)
print(f"{'Processes':<10} {'Size':<10} {'Runtime(s)':<10} {'Efficiency(%)':<10}")
print("-" * 54)
for _, row in df.iterrows():
    print(f"{int(row['processes']):<10} {int(row['size']):<10} {row['runtime']:<10.3f} {row['efficiency']:<10.2f}")