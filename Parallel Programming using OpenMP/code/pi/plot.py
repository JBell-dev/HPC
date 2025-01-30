import matplotlib.pyplot as plt
import numpy as np
import re

def parse(line):
    parts = line.split()
    threads = int(parts[0])
    #i need to eliminate the secs term that got saved in the file
    serial_time = float(re.search(r'time_srl=([\d.e+-]+)', line).group(1))
    parallel_time = float(re.search(r'time_plr=([\d.e+-]+)', line).group(1))
    return threads, serial_time, parallel_time


threads = []
serial_times = []
parallel_times = []
with open('pi_approximation_results.data', 'r') as file:
    next(file)
    for line in file:
        t, s_time, p_time = parse(line)
        threads.append(t)
        serial_times.append(s_time)
        parallel_times.append(p_time)

speedups = [serial_times[0] / p_time for p_time in parallel_times]
efficiencies = [speedup / thread for speedup, thread in zip(speedups, threads)]


fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(12, 5))

# Strong scaling plot
ax1.plot(threads, speedups, 'bo-')
ax1.plot(threads, threads, 'r--', label='Ideal speedup')
ax1.set_xscale('log', base=2)
ax1.set_yscale('log', base=2)
ax1.set_xlabel('Number of Threads')
ax1.set_ylabel('Speedup')
ax1.set_title('Strong Scaling: Speedup vs. Number of Threads')
ax1.legend()
ax1.grid(True)

# parallel effieciency plot
ax2.plot(threads, efficiencies, 'go-')
ax2.set_xscale('log', base=2)
ax2.set_xlabel('Number of Threads')
ax2.set_ylabel('Parallel Efficiency')
ax2.set_title('Parallel Efficiency vs. Number of Threads')
ax2.grid(True)

plt.tight_layout()
plt.savefig('pi_approximation_plots.pdf')
plt.show()

print("Threads | Speedup | Efficiency")
for t, s, e in zip(threads, speedups, efficiencies):
    print(f"{t} | {s} | {e}")