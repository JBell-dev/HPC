import numpy as np
import os

def combine_files(output_file, input_prefix, num_ranks, global_dims, local_dims):
    """
    We combine the multiple binaries into one knowing the local subdivision where:
    - global_dims: Dimensions of the global grid.
    - local_dims: Dimensions of the local grid per rank.
    """
    nx_global, ny_global = global_dims
    nx_local, ny_local = local_dims
    
    # init an empty array for the whole grid
    global_array = np.zeros((nx_global, ny_global), dtype=np.float64)
    ranks_x = nx_global // nx_local
    ranks_y = ny_global // ny_local
    
    for rank in range(num_ranks):
        # we calculate the starting position for each rank
        rank_x = rank // ranks_y
        rank_y = rank % ranks_y
        start_x = rank_x * nx_local
        start_y = rank_y * ny_local
        #read from it 
        input_file = f"{input_prefix}{rank}.bin"
        if not os.path.exists(input_file):
            raise FileNotFoundError(f"Input file '{input_file}' does not exist.")
        #copy into the local array
        local_data = np.fromfile(input_file, dtype=np.float64).reshape((nx_local, ny_local))
        #place it in the global array given the correct positioning 
        global_array[start_x:start_x + nx_local, start_y:start_y + ny_local] = local_data
    
    global_array.tofile(output_file)
    print(f"Combined file written to {output_file}")

if __name__ == "__main__":
    output_file = "combined_output.bin"  # combined file
    input_prefix = "output.bin_rank"        # prefix of the input files
    num_ranks = 2                       # total ranks
    global_dims = (128, 128)            #(nx, ny)
    local_dims = (64, 128)               # (nx_local, ny_local)
    
    combine_files(output_file, input_prefix, num_ranks, global_dims, local_dims)