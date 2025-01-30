import pandas as pd
import matplotlib.pyplot as plt

df = pd.read_csv('scaling_results.csv', usecols=['size', 'threads', 'time', 'iters', 'newton_iter'])

fig, axes = plt.subplots(2, 3, figsize=(15, 10))
axes = axes.flatten()
resolutions = [64, 128, 256, 512, 1024]
for idx, res in enumerate(resolutions):
    res_data = df[df['size'] == res]
    
    axes[idx].plot(res_data['threads'], res_data['time'], 'bo-', linewidth=2)
    axes[idx].set_title(f'Resolution {res}×{res}')
    axes[idx].set_xlabel('Number of Threads')
    axes[idx].set_ylabel('Time (seconds)')
    axes[idx].grid(True)
    
    axes[idx].set_xticks([1, 2, 4, 8, 16])
    axes[idx].set_xticklabels(['1', '2', '4', '8', '16'])
    #axes[idx].set_yscale('log')

axes[-1].remove()
plt.tight_layout()
plt.savefig('strong_scaling.png')  # Save the figure before showing it
plt.show()

print("\nScaling Analysis for Each Resolution:")
print("-" * 60)
for res in resolutions:
    print(f"\nResolution {res}×{res}:")
    print(f"{'Threads':>8} | {'Time (s)':>10} | {'Speedup':>10} | {'Efficiency':>10}")
    print("-" * 60)
    res_data = df[df['size'] == res]
    base_time = float(res_data[res_data['threads'] == 1]['time'].values[0])
    for threads in [1, 2, 4, 8, 16]:
        time = float(res_data[res_data['threads'] == threads]['time'].values[0])
        speedup = base_time / time
        efficiency = speedup / threads
        print(f"{threads:8d} | {time:10.3f} | {speedup:10.3f} | {efficiency:10.3f}")