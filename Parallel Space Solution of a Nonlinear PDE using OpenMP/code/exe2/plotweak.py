import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

df = pd.read_csv('weak_scaling_results.csv')
plt.figure(figsize=(12, 8))
base_resolutions = [64, 128, 256]
markers = ['o', 's', '^'] 
colors = ['blue', 'red', 'green']

for base_res, marker, color in zip(base_resolutions, markers, colors):
    data = df[df['base_resolution'] == base_res]
    plt.plot(data['threads'], data['time'], 
             label=f'Base {base_res}x{base_res}',
             marker=marker, 
             color=color,
             linewidth=2,
             markersize=8)
    #anottations
    for _, row in data.iterrows():
        plt.annotate(f'{int(row["grid_size"])}x{int(row["grid_size"])}',
                    (row['threads'], row['time']),
                    xytext=(5, 5),
                    textcoords='offset points',
                    fontsize=8)

plt.grid(True, which="both", ls="-", alpha=0.2)
plt.yscale('log')          # Log scale for time
plt.xlabel('Number of Threads', fontsize=12)
plt.ylabel('Time (seconds) - log scale', fontsize=12)
plt.title('Weak Scaling Performance\nConstant Work per Thread', fontsize=14)
plt.legend(fontsize=10)
plt.xticks([1, 2, 4, 8, 16], ['1', '2', '4', '8', '16'])
plt.tight_layout()
plt.savefig('weak_scaling_plot.png', dpi=300, bbox_inches='tight')
plt.show()
