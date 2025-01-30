import numpy as np
import matplotlib.pyplot as plt

# https://www.geeksforgeeks.org/import-text-files-into-numpy-arrays/
data = np.genfromtxt('mandelbrot_results.data', skip_header=1)
threads = data[:, 0]
times = data[:, 1]
# same as before
speedup = times[0] / times
efficiency = speedup / threads
#plot
fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(12, 5))
#strong scaling
ax1.plot(threads, speedup, 'bo-')
ax1.plot(threads, threads, 'r--', label='Ideal speedup')
ax1.set_xscale('log', base=2)
ax1.set_yscale('log', base=2)
ax1.set_xlabel('Number of Threads')
ax1.set_ylabel('Speedup')
ax1.set_title('Strong Scaling: Speedup vs. Number of Threads')
ax1.legend()
ax1.grid(True)
#parallel efficiency
ax2.plot(threads, efficiency, 'go-')
ax2.set_xscale('log', base=2)
ax2.set_xlabel('Number of Threads')
ax2.set_ylabel('Parallel Efficiency')
ax2.set_title('Parallel Efficiency vs. Number of Threads')
ax2.grid(True)
plt.tight_layout()
plt.savefig('mandel_plot.pdf')
plt.show()
#to check
print("Threads | Speedup | Efficiency")
for t, s, e in zip(threads, speedup, efficiency):
    print(f"{t} | {s} | {e}")