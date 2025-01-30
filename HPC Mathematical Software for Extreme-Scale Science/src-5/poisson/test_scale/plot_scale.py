import numpy as np
import matplotlib.pyplot as plt

# data from the logs: 
num_processes = np.array([1, 2, 4, 8, 16])
assembly_times = np.array([0.597465, 0.327048, 0.159975, 0.098076, 0.076602])
solve_times = np.array([18.692537, 9.848923, 5.240297, 2.880924, 1.565205])
total_times = assembly_times + solve_times

speedup = total_times[0] / total_times
efficiency = speedup / num_processes

fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(15, 6))
ax1.loglog(num_processes, total_times, 'bo-', label='Total Time', linewidth=2)
ax1.loglog(num_processes, assembly_times, 'rs--', label='Assembly Time', linewidth=2)
ax1.loglog(num_processes, solve_times, 'g^--', label='Solve Time', linewidth=2)
ideal_scaling = total_times[0] / num_processes
ax1.loglog(num_processes, ideal_scaling, 'k:', label='Ideal Scaling', alpha=0.5)

ax1.grid(True, which="both", ls="-", alpha=0.2)
ax1.set_xlabel('Number of Processes', fontsize=12)
ax1.set_ylabel('Runtime (seconds)', fontsize=12)
ax1.set_title('Runtime vs Number of Processes\n(Grid Size: 1024×1024)', fontsize=12)
ax1.legend(fontsize=10)

#Speedup and eff. 
ax2_twin = ax2.twinx()  
line1 = ax2.plot(num_processes, speedup, 'bo-', label='Speedup', linewidth=2)
ax2.plot(num_processes, num_processes, 'k:', label='Ideal Speedup', alpha=0.5)
ax2.set_xlabel('Number of Processes', fontsize=12)
ax2.set_ylabel('Speedup', fontsize=12, color='b')
ax2.tick_params(axis='y', labelcolor='b')
line2 = ax2_twin.plot(num_processes, efficiency, 'rs--', label='Efficiency', linewidth=2)
ax2_twin.set_ylabel('Efficiency', fontsize=12, color='r')
ax2_twin.tick_params(axis='y', labelcolor='r')
lines = line1 + line2
labels = [l.get_label() for l in lines]
ax2.legend(lines, labels, fontsize=10)
ax2.grid(True, alpha=0.2)
ax2.set_title('Parallel Performance Metrics\n(Grid Size: 1024×1024)', fontsize=12)
plt.tight_layout()
plt.savefig('parallel_scaling.png', dpi=300, bbox_inches='tight')

