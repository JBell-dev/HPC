import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

df = pd.read_csv('scaling_results.csv')
df_50 = df[df['tasks'] == 50]
df_100 = df[df['tasks'] == 100]
base_time_50 = df_50.iloc[0]['runtime'] 
base_time_100 = df_100.iloc[0]['runtime']
df_50['speedup'] = base_time_50 / df_50['runtime']
df_100['speedup'] = base_time_100 / df_100['runtime']
plt.figure(figsize=(10, 6))
plt.plot(df_50['workers'], df_50['speedup'], 'o-', label='50 tasks')
plt.plot(df_100['workers'], df_100['speedup'], 's-', label='100 tasks')
plt.plot([2, 16], [1, 8], 'k--', label='Ideal scaling') #the ideal 45 degree line

plt.xlabel('Number of Workers')
plt.ylabel('Speedup')
plt.title('Strong Scaling')
plt.grid(True)
plt.legend()
plt.savefig('scaling_study.pdf')
plt.close()