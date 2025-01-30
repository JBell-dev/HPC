import matplotlib.pyplot as plt
import numpy as np


data = np.loadtxt('dotproduct_results.data')
N_values = np.unique(data[:, 0])
print(data)
# figure config
#https://hpc-wiki.info/hpc/Scaling -> this is how the scaling is calculated
plt.figure(figsize=(12, 8))
plt.title('Strong Scaling Analysis of Dot Product')
plt.xlabel('Number of Threads')
plt.ylabel('Speedup')
plt.xscale('log', base=2)
plt.yscale('log', base=2)
plt.grid(True)

# each curve will be a different N
for N in N_values:
    N_data = data[data[:, 0] == N]    #ordered n by output
    #print(N_data)
    threads = N_data[:, 1] #number of threads also ordered by output
    speedup_reduction = N_data[:, 2] / N_data[:, 3] # t(1) / t(N)
    speedup_critical = N_data[:, 2] / N_data[:, 4]
    plt.plot(threads, speedup_reduction, 'o-', label=f'Reduction N={N:.2e}')
    plt.plot(threads, speedup_critical, 's--', label=f'Critical N={N:.2e}')

plt.legend(bbox_to_anchor=(1.05, 1), loc='upper left')
plt.tight_layout()
plt.savefig('dotproduct_strong_scaling.pdf')
plt.show()