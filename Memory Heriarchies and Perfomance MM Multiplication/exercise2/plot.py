import numpy as np
import matplotlib.pyplot as plt

# values from our calculations
PEAK_PERFORMANCE = 36.8 # GFLOP/s
ORIGIN_INTERCEPT = 0.1 # GFLOP/s -> i assume this from the example in the assignment
PEAK_BANDWIDTH = 11.96 # GB/s
RIDGE_POINT = 3.08 # FLOP/Byte

def attainable_performance(operational_intensity):
    return np.minimum(PEAK_PERFORMANCE, PEAK_BANDWIDTH * operational_intensity)

x = np.logspace(-1, 2, 100) # from 0.1 to 100 
y = attainable_performance(x)

# plot
plt.figure(figsize=(10, 6))
plt.loglog(x, y, label='Roofline')
plt.xlabel('Operational intensity (FLOP/Byte)')
plt.ylabel('Attainable performance (GFLOP/s)')
plt.title('Roofline Model')
plt.xlim(0.1, 100)
plt.ylim(0.1, 100)
plt.grid(True)
plt.text(RIDGE_POINT, PEAK_PERFORMANCE, f'Ridge Point\n({RIDGE_POINT:.2f}, {PEAK_PERFORMANCE:.2f})')
plt.savefig('roofline_plot.png')
plt.show()