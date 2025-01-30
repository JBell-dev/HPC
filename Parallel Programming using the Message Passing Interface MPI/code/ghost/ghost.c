/****************************************************************
 *                                                              *
 * This file has been written as a sample solution to an        *
 * exercise in a course given at the CSCS-USI Summer School     *
 * It is made freely available with the understanding that      *
 * every copy of this file must include this header and that    *
 * CSCS/USI take no responsibility for the use of the enclosed  *
 * teaching material.                                           *
 *                                                              *
 * Purpose: Exchange ghost cell in 2 directions using a topology*
 *                                                              *
 * Contents: C-Source                                           *
 *                                                              *
 ****************************************************************/

/* Use only 16 processes for this exercise
 * Send the ghost cell in two directions: left<->right and top<->bottom
 * ranks are connected in a cyclic manner, for instance, rank 0 and 12 are connected
 *
 * process decomposition on 4*4 grid
 *
 * |-----------|
 * | 0| 1| 2| 3|
 * |-----------|
 * | 4| 5| 6| 7|
 * |-----------|
 * | 8| 9|10|11|
 * |-----------|
 * |12|13|14|15|
 * |-----------|
 *
 * Each process works on a 6*6 (SUBDOMAIN) block of data
 * the D corresponds to data, g corresponds to "ghost cells"
 * xggggggggggx
 * gDDDDDDDDDDg
 * gDDDDDDDDDDg
 * gDDDDDDDDDDg
 * gDDDDDDDDDDg
 * gDDDDDDDDDDg
 * gDDDDDDDDDDg
 * gDDDDDDDDDDg
 * gDDDDDDDDDDg
 * gDDDDDDDDDDg
 * gDDDDDDDDDDg
 * xggggggggggx
 */

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define SUBDOMAIN 6
#define DOMAINSIZE (SUBDOMAIN+2)

int main(int argc, char *argv[])
{
    int rank, size, i, j, dims[2], periods[2], rank_top, rank_bottom, rank_left, rank_right;
    double data[DOMAINSIZE*DOMAINSIZE];
    MPI_Request request[8];
    MPI_Status status[8];
    MPI_Comm comm_cart;
    MPI_Datatype data_ghost;

    // Initialize MPI
    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size!=16) {
        printf("please run this with 16 processors\n");
        MPI_Finalize();
        exit(1);
    }

    // initialize the domain
    for (i=0; i<DOMAINSIZE*DOMAINSIZE; i++) {
        data[i]=rank;
    }

    // TODO: set the dimensions of the processor grid and periodic boundaries in both dimensions
    dims[0]=4; //4x4 grid
    dims[1]=4; //4x4 grid
    periods[0]=1; //periodic in the x direction -> true
    periods[1]=1; //periodic in the y direction -> true
    //notice that in the provided doc of  MPI_cart_create it is suggested :
    //MPI_dims_create since it would calculate the optimal size for the given process size. 
    //https://rookiehpc.org/mpi/docs/mpi_cart_create/index.html

    // TODO: Create a Cartesian communicator (4*4) with periodic boundaries (we do not allow
    // the reordering of ranks) and use it to find your neighboring
    // ranks in all dimensions in a cyclic manner.
    MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, 0, &comm_cart); //notice that 0 for not allowing reordering
    
    // TODO: find your top/bottom/left/right neighbor using the new communicator, see MPI_Cart_shift()
    // rank_top, rank_bottom
    // rank_left, rank_right
    MPI_Cart_shift(comm_cart, 0, 1, &rank_top, &rank_bottom); //vertical neighbors ranks
    MPI_Cart_shift(comm_cart, 1, 1, &rank_left, &rank_right); //horizontal neighbors ranks

    //  TODO: create derived datatype data_ghost, create a datatype for sending the column, see MPI_Type_vector() and MPI_Type_commit()
    // column vector sending of ghost cells
    MPI_Type_vector(SUBDOMAIN, 1, DOMAINSIZE, MPI_DOUBLE, &data_ghost); 
    //where subdomain indicates the number elements in the block to send, 1 is because is an element, downsize is the stride (we need ghost cells only)  
    MPI_Type_commit(&data_ghost);

    //  TODO: ghost cell exchange with the neighbouring cells in all directions
    //  use MPI_Irecv(), MPI_Send(), MPI_Wait() or other viable alternatives

    // From top -> count from 1 to 6 since the corners are not ghost cells for the receiver. 
    MPI_Irecv(&data[1], SUBDOMAIN, MPI_DOUBLE, rank_top, 0, comm_cart, &request[0]);
    // WHERE: we store starting from index 1 for before explained reasons. 
    // HOW MUCH: receive 6 elements (SUBDOMAIN = 6)
    // TYPE: each element is a double
    // FROM WHO: receive from process above us 
    // TAG: message identifier (match with sender tag)
    //https://rookiehpc.org/mpi/docs/mpi_irecv/index.html
    // From bottom
    MPI_Irecv(&data[(DOMAINSIZE)*(DOMAINSIZE-1)+1], SUBDOMAIN, MPI_DOUBLE, rank_bottom, 1, comm_cart, &request[1]);
    //to understand the starting index we can think of 8*7 leave us in the before to last row, +1, beginning of the last row. thats where 9's process elements 
    //should be for 5th process (visualizing my example).
    // From left
    MPI_Irecv(&data[DOMAINSIZE], 1, data_ghost, rank_left, 2, comm_cart, &request[2]);
     //so here we take the first row, then data_ghost will manage for us as we defined before the column vector with the ghost values
    //such that we start from the beginning of the columnm, the pass 8 to end in the beg of the next row, ... till complete the column.
    // From right
    MPI_Irecv(&data[2*DOMAINSIZE-1], 1, data_ghost, rank_right, 3, comm_cart, &request[3]);

    // Then post sends
    // To top-> same idea that before but now from the senders, so for example, we pass the first row of data (not ghost cells) to the top.
    MPI_Isend(&data[DOMAINSIZE+1], SUBDOMAIN, MPI_DOUBLE, rank_top, 1, comm_cart, &request[4]);

    // To bottom
    MPI_Isend(&data[DOMAINSIZE*(DOMAINSIZE-2)+1], SUBDOMAIN, MPI_DOUBLE, rank_bottom, 0, comm_cart, &request[5]);

    // To left
    MPI_Isend(&data[DOMAINSIZE+1], 1, data_ghost, rank_left, 3, comm_cart, &request[6]);

    // To right
    MPI_Isend(&data[DOMAINSIZE+SUBDOMAIN], 1, data_ghost, rank_right, 2, comm_cart, &request[7]);

    // we need to wait for all communications to complete 
    MPI_Waitall(8, request, status);

    if (rank==9) {
        printf("data of rank 9 after communication\n");
        for (j=0; j<DOMAINSIZE; j++) {
            for (i=0; i<DOMAINSIZE; i++) {
                printf("%4.1f ", data[i+j*DOMAINSIZE]);
            }
            printf("\n");
        }
    }

    // TODO
    MPI_Type_free(&data_ghost);
    MPI_Comm_free(&comm_cart);

    // Finalize MPI
    MPI_Finalize();

    return 0;
}