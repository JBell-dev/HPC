from mandelbrot_task import *
import matplotlib as mpl
mpl.use('Agg')
import matplotlib.pyplot as plt
from mpi4py import MPI # MPI_Init and MPI_Finalize automatically called
import numpy as np
import sys
import time

# some parameters
MANAGER = 0       # rank of manager
TAG_TASK      = 1 # task       message tag
TAG_TASK_DONE = 2 # tasks done message tag
TAG_DONE      = 3 # done       message tag

def manager(comm, tasks):
    """
    The manager distributes tasks to workers and collects results.
    
    Parameters
    ----------
    comm : mpi4py.MPI communicator
        MPI communicator
    tasks : list of objects with a do_task() method perfroming the task
        List of tasks to accomplish

    Returns
    -------
    list
        List of completed tasks
    """
    size = comm.Get_size() #need the proccess number
    status = MPI.Status() #need to get the status of the message since we need to receive them
    completed_tasks = [] #to store the finished tasks
    tasks_todo = tasks.copy() #remaining tasks
    tasks_in_progress = 0 #init the number of tasks in progress
    worker_status = [False] * size  # init the iddle state of the workers ( False = idle, True = busy)
    
    global TasksDoneByWorker
    TasksDoneByWorker = np.zeros(size, dtype=int) #init the global count of tasks done by each worker
    
    # tasks distribution
    for worker in range(1, size):
        if tasks_todo:
            task = tasks_todo.pop(0)
            comm.send(task, dest=worker, tag=TAG_TASK)
            worker_status[worker] = True
            tasks_in_progress += 1
    
    # receive results and distribute remaining tasks
    while tasks_in_progress > 0:
        # we need to receive the task from any worker
        task = comm.recv(source=MPI.ANY_SOURCE, tag=TAG_TASK_DONE, status=status) #https://stackoverflow.com/questions/4348900/mpi-recv-from-an-unknown-source
        worker = status.Get_source() #get the worker that finished the task
        TasksDoneByWorker[worker] += 1  # rise up the statistic for this worker 
        worker_status[worker] = False #now idle again
        tasks_in_progress -= 1 #and one less thing to do!
        completed_tasks.append(task) #done tasks 
        
        # if another task appears, we send it to the worker
        if tasks_todo:
            task = tasks_todo.pop(0)
            comm.send(task, dest=worker, tag=TAG_TASK)
            worker_status[worker] = True #working on it...
            tasks_in_progress += 1 #an added to the count 
    
    # now, if nothing else remains then, we tell the workers that we are done
    for worker in range(1, size):
        comm.send(None, dest=worker, tag=TAG_DONE)
        
    return completed_tasks

def worker(comm):
    """
    it worker receives tasks, processes them, and sends results back.

    Parameters
    ----------
    comm : mpi4py.MPI communicator
        MPI communicator
    """
    status = MPI.Status() #need this to get the tag data, to see if we should break.
    
    while True:
        # it waits for manager orders
        task = comm.recv(source=MANAGER, tag=MPI.ANY_TAG, status=status)
        if status.Get_tag() == TAG_DONE: # and we need to check if its done to stop 
            break  
        # we do the work with the provided implementation.
        task.do_work()
        comm.send(task, dest=MANAGER, tag=TAG_TASK_DONE) # and we send the result back

def readcmdline(rank):
    """
    Read command line arguments

    Parameters
    ----------
    rank : int
        Rank of calling MPI process

    Returns
    -------
    nx : int
        number of gridpoints in x-direction
    ny : int
        number of gridpoints in y-direction
    ntasks : int
        number of tasks
    """
    # report usage
    if len(sys.argv) != 4:
        if rank == MANAGER:
            print("Usage: manager_worker nx ny ntasks")
            print("  nx     number of gridpoints in x-direction")
            print("  ny     number of gridpoints in y-direction")
            print("  ntasks number of tasks")
        sys.exit()

    # read nx, ny, ntasks
    nx = int(sys.argv[1])
    if nx < 1:
        sys.exit("nx must be a positive integer")
    ny = int(sys.argv[2])
    if ny < 1:
        sys.exit("ny must be a positive integer")
    ntasks = int(sys.argv[3])
    if ntasks < 1:
        sys.exit("ntasks must be a positive integer")

    return nx, ny, ntasks


if __name__ == "__main__":
    # get COMMON WORLD communicator, size & rank
    comm    = MPI.COMM_WORLD
    size    = comm.Get_size()
    my_rank = comm.Get_rank()

    # report on MPI environment
    if my_rank == MANAGER:
        print(f"MPI initialized with {size:5d} processes")

    # read command line arguments
    nx, ny, ntasks = readcmdline(my_rank)

    # start timer
    timespent = - time.perf_counter()

    # tasks creation
    x_min = -2.
    x_max = +1.
    y_min = -1.5
    y_max = +1.5
    M = mandelbrot(x_min, x_max, nx, y_min, y_max, ny, ntasks)
    tasks = M.get_tasks() # we get the tasks with the provided implementation 

    if my_rank == MANAGER:
        completed_tasks = manager(comm, tasks)
        m = M.combine_tasks(completed_tasks)
        plt.imshow(m.T, cmap="gray", extent=[x_min, x_max, y_min, y_max])
        plt.savefig("mandelbrot.png")
    else:
        worker(comm)

    # stop timer
    timespent += time.perf_counter()

    # inform that done
    if my_rank == MANAGER:
        print(f"Run took {timespent:f} seconds")
        for i in range(size):
            if i == MANAGER:
                continue
            print(f"Process {i:5d} has done {TasksDoneByWorker[i]:10d} tasks")
        print("Done.")
