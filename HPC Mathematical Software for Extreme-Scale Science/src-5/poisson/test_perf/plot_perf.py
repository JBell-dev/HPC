import numpy as np
import matplotlib.pyplot as plt

#I EXTRACT THIS FROM THE LOGS: 
grid_sizes = np.array([8, 16, 32, 64, 128, 256, 512])

# PETSc 
petsc_assembly = np.array([0.001087, 0.001137, 0.001523, 0.003005, 0.008771, 0.033469, 0.133794])
petsc_solve = np.array([0.000048, 0.000089, 0.000474, 0.003292, 0.026401, 0.207403, 1.961334])
petsc_total = petsc_assembly + petsc_solve

# Python 
python_assembly = np.array([0.000607, 0.001093, 0.003030, 0.011163, 0.043756, 0.179478, 0.753097])
python_sparse = np.array([0.000279, 0.000749, 0.003100, 0.015249, 0.103832, 0.786240, 7.344012])
python_cg = np.array([0.000417, 0.000947, 0.002347, 0.008315, 0.059424, 0.376172, 2.991864])

# log-log plot
plt.figure(figsize=(12, 8))
plt.loglog(grid_sizes, petsc_total, 'bo-', label='PETSc Total', linewidth=2)
plt.loglog(grid_sizes, petsc_assembly, 'b--', label='PETSc Assembly', alpha=0.7)
plt.loglog(grid_sizes, petsc_solve, 'b:', label='PETSc Solve', alpha=0.7)
plt.loglog(grid_sizes, python_sparse, 'ro-', label='Python Sparse Direct', linewidth=2)
plt.loglog(grid_sizes, python_cg, 'go-', label='Python CG', linewidth=2)
plt.loglog(grid_sizes, python_assembly, 'k--', label='Python Assembly', alpha=0.7)

plt.grid(True, which="both", ls="-", alpha=0.2)
plt.xlabel('Grid Size (N×N)', fontsize=12)
plt.ylabel('Runtime (seconds)', fontsize=12)
plt.title('Performance Comparison: PETSc vs Python Implementations', fontsize=14)
plt.legend(fontsize=10)
plt.tight_layout()
plt.savefig('runtime_comparison.png', dpi=300, bbox_inches='tight')

