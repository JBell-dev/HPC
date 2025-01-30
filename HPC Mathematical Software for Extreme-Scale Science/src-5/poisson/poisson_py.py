"""
 * HPC Assignment: Python Poisson Equation Solver
 * Author: Aryan Eftekhari
 *
 * Description:
 * This Python program solves the 2D Poisson equation using finite difference methods. 
 * It constructs a sparse Laplacian matrix and solves the resulting linear system 
 * using various solvers (e.g., sparse direct, dense direct, and conjugate gradient methods).
 * 
 * The equation solved is:
 *     -Δu = f, 0 < x, y < 1
 * with:
 *     - Dirichlet boundary conditions (u = 0 on all boundaries).
 *     - Forcing function f = constant.
 *
 * Fix Suggestion:
 * Ensure input arguments are validated for edge cases. Include more solver options and refine grid scaling for large `nx` and `ny`.
 *
 * Compilation Instructions:
 * No compilation required. Ensure required Python libraries (`numpy`, `scipy`, `argparse`) are installed.
 * 
 * Run Instructions:
 * Execute the script with desired parameters. Example:
 *     python poisson_solver.py -nx 64 -ny 64 -solver sp_dir,dn_dir,sp_cg
 * where sp_dir:  Sparse Direct Solver, dn_dir: Dense Direct Solver, sp_cg: Conjugate Gradient Solver
 """

import numpy as np
from scipy.sparse import csr_matrix
from scipy.sparse.linalg import spsolve, cg
from numpy.linalg import solve
import time9
import argparse


def ComputeRHS(nx, ny, f_value):
    """Compute the right-hand side vector."""
    rhs = np.zeros((ny, nx), dtype=float) 
    
    # we need to set the interior points to f_value where i=1,...,nx-2 and j=1,...,ny-2
    rhs[1:ny-1, 1:nx-1] = f_value
    #row major ordering: 
    return rhs.flatten()


def ComputeMatrix(nx, ny, dx, dy):
    N = nx * ny
    data = []
    row_indices = []
    col_indices = []

    hx2 = (dx*dx)
    hy2 = (dy*dy)

    for j in range(ny):
        for i in range(nx):
            row = i + j*nx

            # we first check the boundary case
            if i == 0 or i == nx-1 or j == 0 or j == ny-1:
                # given the conditions u=0 we want Arow,row = 1 since then 1  * ui,j + 0 (the other terms ) = ui,j 
                # and then by setting brow = 0 give us ui,j = 0 on that boudnary position that is what we want
                data.append(1.0)
                row_indices.append(row)
                col_indices.append(row)
            else:
                # For the interior points: 
                # Diagonal
                diag_val = 2.0*(1.0/hx2 + 1.0/hy2) #managing for cases of non uniform grid spacing 

                data.append(diag_val)
                row_indices.append(row)
                col_indices.append(row)

                # left neighbor
                if i > 0:
                    data.append(-1.0/hx2)
                    row_indices.append(row)
                    col_indices.append(row - 1)

                # right neighbor
                if i < nx-1:
                    data.append(-1.0/hx2)
                    row_indices.append(row)
                    col_indices.append(row + 1)

                # bottom neighbor
                if j > 0:
                    data.append(-1.0/hy2)
                    row_indices.append(row)
                    col_indices.append(row - nx)

                # top neighbor
                if j < ny-1:
                    data.append(-1.0/hy2)
                    row_indices.append(row)
                    col_indices.append(row + nx)

    return csr_matrix((data, (row_indices, col_indices)), shape=(N, N))


if __name__ == "__main__":
    # Parse command-line arguments
    parser = argparse.ArgumentParser(description="Solve the Poisson equation using different solvers.")
    parser.add_argument("-nx", type=int, default=64, help="Number of grid points in x (and y).")
    parser.add_argument("-ny", type=int, default=18,  help="Number of grid points in y (default: same as nx).")
    parser.add_argument("-solver", type=str, default="sp_dir,dn_dir,sp_cg",  help="Comma-separated list of solvers to use.")
    args = parser.parse_args()

    # Parameters
    nx = args.nx
    ny = args.ny

    solver_list = args.solver.split(",")
    print("Selected solvers:", solver_list)

    Lx, Ly = 1.0, 1.0  # Domain size
    dx = Lx / (nx - 1) # Grid spacing in x
    dy = Ly / (ny - 1) # Grid spacing in y
    f_value = 20.0     # Right-hand side (constant)

    print("-"*100)
    print(f"Poisson's Equation Solver Python for {nx}x{ny}")

    # Compute RHS and Matrix
    start_rhs = time.time()
    rhs = ComputeRHS(nx, ny, f_value)
    end_rhs = time.time()

    start_matrix = time.time()
    laplacian = ComputeMatrix(nx, ny, dx, dy)
    end_matrix = time.time()

    np.set_printoptions(linewidth=200)

    print("-"*100)
    print(f"{'RHS Time:':<40} {end_rhs - start_rhs:.6f} seconds")
    print(f"{'Matrix Assembly Time:':<40} {end_matrix - start_matrix:.6f} seconds")
    print("-"*100)
    # Solve the system with different methods
    solutions = {}

    if 'sp_dir' in solver_list:
        # Sparse direct solver
        start_sparse_direct = time.time()
        solutions['sp_dir'] = spsolve(laplacian, rhs)
        end_sparse_direct = time.time()
        print(f"{'Sparse Direct Solver Time:':<40} {end_sparse_direct - start_sparse_direct:.6f} seconds, "
            f"Norm of Solution: {np.linalg.norm(solutions['sp_dir'], ord=2):.6f}")

    if 'dn_dir' in solver_list:
        # Dense direct solver
        dense_laplacian = laplacian.toarray()
        start_dense_direct = time.time()
        solutions['dn_dir'] = solve(dense_laplacian, rhs)
        end_dense_direct = time.time()
        print(f"{'Dense Direct Solver Time:':<40} {end_dense_direct - start_dense_direct:.6f} seconds, "
            f"Norm of Solution: {np.linalg.norm(solutions['dn_dir'], ord=2):.6f}")

    if 'sp_cg' in solver_list:
        # Sparse iterative solver (Conjugate Gradient)
        start_cg = time.time()
        solutions['sp_cg'], _ = cg(laplacian, rhs, tol=1e-6)
        end_cg = time.time()
        print(f"{'Conjugate Gradient (sparse) Solver Time:':<40} {end_cg - start_cg:.6f} seconds, "
            f"Norm of Solution: {np.linalg.norm(solutions['sp_cg'], ord=2):.6f}")

    #Write the solution vector to a text file.
    filename = "solution_py_sp_dir"

    print("-"*100)
    for solver in solver_list:
        filename = "solution_"+solver
        with open(filename+'.dat', 'w') as file:
            file.write("Py Solution\n")
            file.write("============\n")
            np.savetxt(file, solutions[solver], fmt='%.8f')

        with open(filename+'.info', 'w') as file:
            file.write("# nx, ny, a\n")
            file.write(f"{nx},{ny},-\n")

        print(f"Solution written to disk for {filename} ...")
