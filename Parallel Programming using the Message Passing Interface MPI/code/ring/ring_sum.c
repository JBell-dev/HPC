#include <mpi.h> // MPI
#include <stdio.h>

int main(int argc, char *argv[]) {

  // Initialize MPI, get size and rank
  int size, rank;
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  // IMPLEMENT: Ring sum algorithm
  int sum = rank; // initialize sum with the rank number since "every process initially sends its rank number to a neighbor"
  int next = (rank + 1) % size; // this allow as to manage the edge case on the last rank -> "upper" neighbor
  int prev = (rank - 1 + size) % size; //manage the edge case on the first rank -> "lower" neighbor
  int send_message = rank; // then, the first message is the rank number of the node. 
  int recv_message; //will store the received message from the neighbor
  for (int i = 0; i < size-1; i++) {
    //one idea to avoid a deadlock as in example "point_to_point" given in class, is to use the properties of the nodes ids
    //to send first the even ranks with the odd ranks receiving and vice versa.
    //out of curiosity, i also tried without this thinking that it will not work since the assignment tell us to take care of deadlocks,
    //but surprisingly it worked anyway! i dont know if mpi manage this in some way. 
    if (rank % 2 == 0) { 
      MPI_Send(&send_message, 1, MPI_INT, next, 0, MPI_COMM_WORLD); //https://hpc-tutorials.llnl.gov/mpi/routine_args/
      MPI_Recv(&recv_message, 1, MPI_INT, prev, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    } else {
      MPI_Recv(&recv_message, 1, MPI_INT, prev, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      MPI_Send(&send_message, 1, MPI_INT, next, 0, MPI_COMM_WORLD);
    }
    sum += recv_message; //update the sum with the received message
    send_message = recv_message; //update the message to be sent with the received message
  }
  printf("Process %i: Sum = %i\n", rank, sum);

  // Finalize MPI
  MPI_Finalize();

  return 0;
}
