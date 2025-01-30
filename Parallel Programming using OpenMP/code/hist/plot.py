import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

# i manually pass the data we get
threads = [2**i for i in range(8)] + [0]   # 0 represents the sequential run
times = [0.828707, 0.417391, 0.213382, 0.109891, 0.112772, 0.119933, 0.127336, 0.129399, 0.835593]
print(threads)


df = pd.DataFrame({'Threads': threads, 'Time': times})
df = df.sort_values('Threads')
#as always
serial_time = df[df['Threads'] == 0]['Time'].values[0]
df['Speedup'] = serial_time / df['Time']
df['Efficiency'] = df['Speedup'] / df['Threads']

#plot
plt.figure(figsize=(12, 8))
plt.subplot(2, 2, 1)
plt.plot(df['Threads'][df['Threads'] > 0], df['Time'][df['Threads'] > 0], marker='o')
plt.xscale('log', base=2)
plt.xlabel('Number of Threads')
plt.ylabel('Runtime in sec')
plt.title('Runtime vs Number of Threads')

plt.subplot(2, 2, 2)
plt.plot(df['Threads'][df['Threads'] > 0], df['Speedup'][df['Threads'] > 0], marker='o')
plt.xscale('log', base=2)
plt.yscale('log', base=2)
plt.xlabel('Number of Threads')
plt.ylabel('Speedup')
plt.title('Speedup vs Number of Threads')

plt.subplot(2, 2, 3)
plt.plot(df['Threads'][df['Threads'] > 0], df['Efficiency'][df['Threads'] > 0], marker='o')
plt.xscale('log', base=2)
plt.xlabel('Number of Threads')
plt.ylabel('Efficiency')
plt.title('Efficiency vs Number of Threads')
plt.tight_layout()
plt.savefig('performance_analysis.png')
plt.show()
print(df)
