import pandas as pd
import matplotlib.pyplot as plt

df = pd.read_csv('weak_scaling.csv')
df['Runtime'] = df['Runtime'].str.replace(r'[^\d.]', '', regex=True).astype(float)
base_64 = df.iloc[0:5]
base_128 = df.iloc[5:10]
base_256 = df.iloc[10:15]


plt.figure(figsize=(10, 6))
plt.plot(base_64['Processes'], base_64['Runtime'], 
         'o-', label='Base Grid 64×64', linewidth=2)
plt.plot(base_128['Processes'], base_128['Runtime'], 
         's-', label='Base Grid 128×128', linewidth=2)
plt.plot(base_256['Processes'], base_256['Runtime'], 
         '^-', label='Base Grid 256×256', linewidth=2)
plt.xscale('log', base=2)
plt.yscale('log')
plt.grid(True, which="both", ls="-", alpha=0.2)
plt.xlabel('Number of Processes')
plt.ylabel('Time to Solution')
plt.title('Weak Scaling Performance')
plt.legend(loc='best')

#adding annotations to keep track of the calculated grid size
for series in [base_64, base_128, base_256]:
    for i, row in series.iterrows():
        plt.annotate(f'{int(row["Grid_Size"])}×{int(row["Grid_Size"])}', 
                    (row['Processes'], row['Runtime']),
                    xytext=(5, 5), textcoords='offset points',
                    fontsize=8)
plt.tight_layout()
plt.savefig('weak_scaling.png', dpi=300)
plt.show()