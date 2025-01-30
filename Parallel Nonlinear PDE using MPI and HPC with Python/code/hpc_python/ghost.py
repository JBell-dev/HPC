from mpi4py import MPI
import numpy as np

def main():
    #i first initialize the communicator and get the rank and size
    comm = MPI.COMM_WORLD 
    rank = comm.Get_rank() #the rank id
    size = comm.Get_size() #we get the total number of processes as passed

    # using the MPI.Compute_dims function to compute the dimensions of the grid evenly 
    dims = MPI.Compute_dims(size, [0, 0])  # since dims receives the total number of processes and the arrays of dimensions, passing 0,0 tells mpi to compute itself 
    #example -> if size 4 then dims = [2,2]
    periods = [True, True]  # i make it periodic in both directions
    reorder = False #i don't want to reorder the processes, anyway it is passed false by default but to be complete: https://mpi4py.readthedocs.io/en/4.0.0/reference/mpi4py.MPI.Intracomm.html#mpi4py.MPI.Intracomm.Create_cart
    
    # cartesian communicator
    cart_comm = comm.Create_cart(dims=dims, periods=periods, reorder=reorder)
    
    # then, we need to get each coordinates
    #as always we start with the linear process list [1,2,3,4], we got the grid shape [2,2] and now map each coordinate to each in a row major manner. [[0,0],[0,1],[1,0],[1,1]]
    coords = cart_comm.Get_coords(rank) #for each process, according to its rank, we get the coordinates
    #so then this means that the neighbours of rank 0 for example are the neighbours of [0,0] 
    #which means in a 2x2 that north is [-1,0] (up but since periodic -> down ), south [1,0], east [0,1], west [0,-1] (since periodic -> same that east)
    #then, we get_cart_rank we get the domain elem that mapped to that codomain 
    neighbors = {
        'north': cart_comm.Get_cart_rank((coords[0]-1, coords[1])), 
        'south': cart_comm.Get_cart_rank((coords[0]+1, coords[1])),
        'east': cart_comm.Get_cart_rank((coords[0], coords[1]+1)),
        'west': cart_comm.Get_cart_rank((coords[0], coords[1]-1))
    }
    print(f"process {rank} and its coords={coords} with neighbors={neighbors}")
    
    # now, we create a buffer to store the ranks that we receive from the neighbours
    recv_ranks = {'north': None, 'south': None, 'east': None, 'west': None}
    requests = []

    #notice i use the buffer-provider objects (uppercase methods) for speedup
    # first receives as good practice to avoid deadlock - anyway i use non blocking 
    for direction, neighbor_rank in neighbors.items():
        recv_ranks[direction] = np.empty(1, dtype='i') #storage buffer for each direction 
        req = cart_comm.Irecv(recv_ranks[direction], source=neighbor_rank, tag=0) #we receive the ranks from the neighbours 
        requests.append(req) 
    
    # then the sending part
    send_rank = np.array(rank, dtype='i') #each rank sends its rank to its neighbours
    for neighbor_rank in neighbors.values():
        req = cart_comm.Isend(send_rank, dest=neighbor_rank, tag=0)
        requests.append(req)
    
    # then we wait for all communications to complete
    MPI.Request.Waitall(requests)
    cart_comm.Barrier()
    
    print(f"Process {rank} received: north={recv_ranks['north'][0]}, "
          f"south={recv_ranks['south'][0]}, east={recv_ranks['east'][0]}, "
          f"west={recv_ranks['west'][0]}")

if __name__ == "__main__":
    main()