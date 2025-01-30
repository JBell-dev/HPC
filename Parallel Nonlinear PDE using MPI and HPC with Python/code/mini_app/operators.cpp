//******************************************
// operators.cpp
// based on min-app code written by Oliver Fuhrer, MeteoSwiss
// modified by Ben Cumming, CSCS
// *****************************************

// Description: Contains simple operators which can be used on 2/3d-meshes

#include "data.h"
#include "operators.h"
#include "stats.h"
#include <mpi.h>

namespace operators {

// compute the diffusion-reaction stencils
// s_old is the population concentration at time step k-1, s_new at k,
// and f is the residual (see Eq. (7) in Project 3).
void diffusion(data::Field const& s_old, data::Field const& s_new,
               data::Field& f) {
    using data::options;
    using data::domain;

    using data::bndE;
    using data::bndW;
    using data::bndN;
    using data::bndS;

    using data::buffE;
    using data::buffW;
    using data::buffN;
    using data::buffS;

    double alpha = options.alpha;
    double beta = options.beta;

    int nx = domain.nx;
    int ny = domain.ny;
    int iend  = nx - 1;
    int jend  = ny - 1;

    // TODO: exchange the ghost cells using non-blocking point-to-point
    //       communication
    //each point needs info from its neighbours 
    MPI_Status statuses[8]; //so status for exampe needs to store 2 - one send and one receive per each cardinal direction - N,S,E,W
    MPI_Request requests[8]; //same idea here
    int num_requests = 0;


    // North neighbor exchange
    if (domain.neighbour_north >= 0) {
        // we receive from north -> non blocking receive first 
        MPI_Irecv(&bndN[0], nx, MPI_DOUBLE, domain.neighbour_north, 
                  domain.neighbour_north, MPI_COMM_WORLD, &requests[num_requests++]);
        
        // copy into north buffer
        for (int i = 0; i < nx; i++) 
            buffN[i] = s_new(i, ny-1);
        
        // send to north -> and post the non blocking send
        MPI_Isend(&buffN[0], nx, MPI_DOUBLE, domain.neighbour_north,
                  domain.rank, MPI_COMM_WORLD, &requests[num_requests++]);
    }

    // South neighbor exchange
    if (domain.neighbour_south >= 0) {
        // we receive from south
        MPI_Irecv(&bndS[0], nx, MPI_DOUBLE, domain.neighbour_south,
                  domain.neighbour_south, MPI_COMM_WORLD, &requests[num_requests++]);
        
        // copy into south buffer
        for (int i = 0; i < nx; i++)
            buffS[i] = s_new(i, 0);
        
        // send to south
        MPI_Isend(&buffS[0], nx, MPI_DOUBLE, domain.neighbour_south,
                  domain.rank, MPI_COMM_WORLD, &requests[num_requests++]);
    }

    // East neighbor exchange
    if (domain.neighbour_east >= 0) {
        // we receive from east
        MPI_Irecv(&bndE[0], ny, MPI_DOUBLE, domain.neighbour_east,
                  domain.neighbour_east, MPI_COMM_WORLD, &requests[num_requests++]);
        
        // copy into east buffer
        for (int j = 0; j < ny; j++)
            buffE[j] = s_new(nx-1, j);
        
        // send to east
        MPI_Isend(&buffE[0], ny, MPI_DOUBLE, domain.neighbour_east,
                  domain.rank, MPI_COMM_WORLD, &requests[num_requests++]);
    }

    // West neighbor exchange
    if (domain.neighbour_west >= 0) {
        // we receive from west
        MPI_Irecv(&bndW[0], ny, MPI_DOUBLE, domain.neighbour_west,
                  domain.neighbour_west, MPI_COMM_WORLD, &requests[num_requests++]);
        
        // copy into west buffer
        for (int j = 0; j < ny; j++)
            buffW[j] = s_new(0, j);
        
        // send to west
        MPI_Isend(&buffW[0], ny, MPI_DOUBLE, domain.neighbour_west,
                  domain.rank, MPI_COMM_WORLD, &requests[num_requests++]);
    }

    //as noticable with the loop boundaries, we do not need the ghost cells here
    // the interior grid points
    for (int j=1; j < jend; j++) {
        for (int i=1; i < iend; i++) {
            f(i,j) = -(4. + alpha) * s_new(i,j)     // central point
                   + s_new(i-1,j) + s_new(i+1,j)    // east and west
                   + s_new(i,j-1) + s_new(i,j+1)    // north and south
                   + alpha * s_old(i,j)
                   + beta * s_new(i,j) * (1.0 - s_new(i,j));
        }
    }

    // wait for all communications to complete
    MPI_Waitall(num_requests, requests, statuses);
    //notice that we need to add this since the boundary points depend on the ghost cell values 

    // east boundary
    {
        int i = nx - 1;
        for (int j = 1; j < jend; j++) {
            f(i,j) = -(4. + alpha) * s_new(i,j)
                   + s_new(i-1,j) + bndE[j]
                   + s_new(i,j-1) + s_new(i,j+1)
                   + alpha * s_old(i,j)
                   + beta * s_new(i,j) * (1.0 - s_new(i,j));
        }
    }

    // west boundary
    {
        int i = 0;
        for (int j = 1; j < jend; j++) {
            f(i,j) = -(4. + alpha) * s_new(i,j)
                   + bndW[j]      + s_new(i+1,j)
                   + s_new(i,j-1) + s_new(i,j+1)
                   + alpha * s_old(i,j)
                   + beta * s_new(i,j) * (1.0 - s_new(i,j));
        }
    }

    // north boundary (plus NE and NW corners)
    {
        int j = ny - 1;

        {
            int i = 0; // NW corner
            f(i,j) = -(4. + alpha) * s_new(i,j)
                   + bndW[j]      + s_new(i+1,j)
                   + s_new(i,j-1) + bndN[i]
                   + alpha * s_old(i,j)
                   + beta * s_new(i,j) * (1.0 - s_new(i,j));
        }

        // north boundary
        for (int i = 1; i < iend; i++) {
            f(i,j) = -(4. + alpha) * s_new(i,j)
                   + s_new(i-1,j) + s_new(i+1,j)
                   + s_new(i,j-1) + bndN[i]
                   + alpha * s_old(i,j)
                   + beta * s_new(i,j) * (1.0 - s_new(i,j));
        }

        {
            int i = nx - 1; // NE corner
            f(i,j) = -(4. + alpha) * s_new(i,j)
                   + s_new(i-1,j) + bndE[j]
                   + s_new(i,j-1) + bndN[i]
                   + alpha * s_old(i,j)
                   + beta * s_new(i,j) * (1.0 - s_new(i,j));
        }
    }

    // south boundary (plus SW and SE corners)
    {
        int j = 0;
        {
            int i = 0; // SW corner
            f(i,j) = -(4. + alpha) * s_new(i,j)
                   + bndW[j] + s_new(i+1,j)
                   + bndS[i] + s_new(i,j+1)
                   + alpha * s_old(i,j)
                   + beta * s_new(i,j) * (1.0 - s_new(i,j));
        }

        // south boundary
        for (int i = 1; i < iend; i++) {
            f(i,j) = -(4. + alpha) * s_new(i,j)
                   + s_new(i-1,j) + s_new(i+1,j)
                   + bndS[i]      + s_new(i,j+1)
                   + alpha * s_old(i,j)
                   + beta * s_new(i,j) * (1.0 - s_new(i,j));
        }

        {
            int i = nx - 1; // SE corner
            f(i,j) = -(4. + alpha) * s_new(i,j)
                   + s_new(i-1,j) + bndE[j]
                   + bndS[i]      + s_new(i,j+1)
                   + alpha * s_old(i,j)
                   + beta * s_new(i,j) * (1.0 - s_new(i,j));
        }
    }

    // Accumulate the flop counts
    // 8 ops total per point
    stats::flops_diff += 12 * (nx - 2) * (ny - 2) // interior points
                      +  11 * (nx - 2  +  ny - 2) // NESW boundary points
                      +  11 * 4;                  // corner points
}

}
