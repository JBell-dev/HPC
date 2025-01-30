import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

df = pd.read_csv('strong_scaling.csv')
df['Runtime'] = df['Runtime'].str.replace(r'[^\d.]', '', regex=True).astype(float) #remove the ##
processes = sorted(df['Processes'].unique()) #processes
grid_sizes = sorted(df['Grid_Size'].unique()) #grid sizes
markers = ['o', 's', '^', 'D', 'v']  #markers for each


plt.figure(figsize=(10, 6))
# one line per each (loglog)
for i, p in enumerate(processes):
    data = df[df['Processes'] == p]
    plt.loglog(data['Grid_Size'], data['Runtime'], 
              marker=markers[i], linestyle='-', 
              label=f'{p} Process{"es" if p>1 else ""}')

plt.xlabel('Grid Size')
plt.ylabel('Time to Solution')
plt.title('Scaling Performance for different Grid Sizes')
plt.legend()
plt.tight_layout()
plt.savefig('strong_scaling.png')
plt.show()

# csv with the data:
results = []
for size in grid_sizes:
    base_time = df[(df['Grid_Size'] == size) & (df['Processes'] == 1)]['Runtime'].values[0] #base line
    for p in processes:
        time = df[(df['Grid_Size'] == size) & (df['Processes'] == p)]['Runtime'].values[0]
        speedup = base_time / time
        efficiency = speedup / p
        results.append({
            'Grid_Size': size,
            'Processes': p,
            'Runtime': time,
            'Speedup': speedup,
            'Efficiency': efficiency
        })

results_df = pd.DataFrame(results)
results_df.to_csv('scaling_analysis_results.csv', index=False)
