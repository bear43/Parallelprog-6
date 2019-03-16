#include <mpi/mpi.h>
#include <stdio.h>
#include <memory.h>
#define NUM_DIMS 1
#define BUFFER_SIZE 64
#define BUFFER_VALUE "DJ-eban"

int main(int argc, char **argv)
{
    int rank, size, dims[NUM_DIMS];
    int periods[NUM_DIMS],source, dest;
    int reorder=0;
    char buffer[BUFFER_SIZE];
    MPI_Comm comm_cart;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    memset(dims, 0, sizeof(dims));
    memset(periods, true, sizeof(dims));
    MPI_Dims_create(size, NUM_DIMS, dims);
    MPI_Cart_create(MPI_COMM_WORLD, NUM_DIMS, dims, periods, reorder, &comm_cart);
    MPI_Cart_shift(comm_cart, 0, 1, &source, &dest);
    int prev;
    MPI_Cart_shift(comm_cart, 0, -1, &source, &prev);
    if(rank == 0)
    {
        strcpy(buffer, BUFFER_VALUE);
        MPI_Send(buffer, BUFFER_SIZE, MPI_CHAR, dest, 0, comm_cart);
        MPI_Recv(buffer, BUFFER_SIZE, MPI_CHAR, prev, 0, comm_cart, MPI_STATUS_IGNORE);
        printf("rank=0 Buffer=%s\n", buffer);
        fflush(stdout);
    }
    else
    {
        MPI_Recv(buffer, BUFFER_SIZE, MPI_CHAR, prev, 0, comm_cart, MPI_STATUS_IGNORE);
        printf("rank=%d Buffer=%s\n", rank, buffer);
        fflush(stdout);
        MPI_Send(buffer, BUFFER_SIZE, MPI_CHAR, dest, 0, comm_cart);
    }
    MPI_Comm_free(&comm_cart);
    MPI_Finalize();
    return 0;
}