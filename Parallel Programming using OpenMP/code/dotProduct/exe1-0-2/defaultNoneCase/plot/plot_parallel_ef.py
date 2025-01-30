import matplotlib.pyplot as plt
import numpy as np

#source: https://cvw.cac.cornell.edu/parallel/efficiency/about-efficiency
# Efficiency = Speedup / Number of Processors
data = np.loadtxt('dotproduct_results.data')
N_values = np.unique(data[:, 0])

plt.figure(figsize=(12, 8))
plt.title('Parallel Efficiency Analysis of Dot Product')
plt.xlabel('Number of Threads')
plt.ylabel('Parallel Efficiency')
plt.xscale('log', base=2)
plt.grid(True)

for N in N_values:
    N_data = data[data[:, 0] == N]
    threads = N_data[:, 1]
    efficiency_reduction = (N_data[:, 2] / N_data[:, 3]) / threads
    efficiency_critical = (N_data[:, 2] / N_data[:, 4]) / threads
    plt.plot(threads, efficiency_reduction, 'o-', label=f'Reduction N={N:.2e}')
    plt.plot(threads, efficiency_critical, 's--', label=f'Critical N={N:.2e}')

plt.legend(bbox_to_anchor=(1.05, 1), loc='upper left')
plt.ylim(0, 1)  
plt.tight_layout()
plt.savefig('dotproduct_parallel_efficiency.pdf')
plt.show()