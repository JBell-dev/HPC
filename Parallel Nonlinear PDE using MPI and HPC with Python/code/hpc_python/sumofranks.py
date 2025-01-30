from mpi4py import MPI
import numpy as np
import time

def main():
    comm = MPI.COMM_WORLD
    rank = comm.Get_rank()
    size = comm.Get_size()

    # Using pickle-based communication (lowercase methods) #########################################
    start_time = time.time()
    sum_ranks_pickle = comm.allreduce(rank, op=MPI.SUM)
    end_time = time.time()
    pickle_time = end_time - start_time
    
    if rank == 0:
        print("\nPickle-based communication (lowercase):")
        print(f"Sum of ranks = {sum_ranks_pickle}")
        print(f"Time taken: {pickle_time:.6f} seconds")
    
    # Using buffer-provider objects (uppercase methods) ##############################################
    start_time = time.time()
    rank_array = np.array(rank, dtype=np.int32)
    sum_ranks_array = np.zeros(1, dtype=np.int32)
    comm.Allreduce(rank_array, sum_ranks_array, op=MPI.SUM)
    end_time = time.time()
    buffer_time = end_time - start_time
    
    if rank == 0:
        print("\nBuffer-provider objects (uppercase):")
        print(f"Sum of ranks = {sum_ranks_array[0]}")
        print(f"Time taken: {buffer_time:.6f} seconds")

if __name__ == "__main__":
    main()