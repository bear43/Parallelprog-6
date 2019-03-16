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
    MPI_Status status;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    /* Обнуляем массив dims и заполняем массив periods для топологии "кольцо" */
    memset(dims, 0, sizeof(dims));
    memset(periods, true, sizeof(dims));
    /* Заполняем массив dims, в котором указываются размеры решетки */
    MPI_Dims_create(size, NUM_DIMS, dims);
    /* Создаем топологию "кольцо" с communicator-ом comm_cart */
    MPI_Cart_create(MPI_COMM_WORLD, NUM_DIMS, dims, periods, reorder, &comm_cart);
    /* Каждая ветвь находит своих соседей вдоль кольца, в направлении больших значений рангов */
    MPI_Cart_shift(comm_cart, 0, 1, &source, &dest);
    /* Каждая ветвь передает свои данные (значение переменной A) соседней ветви
    с большим рангом и принимает данные в B от соседней ветви с меньшим рангом.
    Ветвь с рангом size-1 передает свои данные ветви с рангом 0, а ветвь 0 принимает
    данные от ветви size-1. */
    if(rank == 0)
    {
        strcpy(buffer, BUFFER_VALUE);
        MPI_Send(buffer, BUFFER_SIZE, MPI_CHAR, dest, 0, comm_cart);
        MPI_Recv(buffer, BUFFER_SIZE, MPI_CHAR, size-1, 0, comm_cart, MPI_STATUS_IGNORE);
    }
    else
    {
        MPI_Recv(buffer, BUFFER_SIZE, MPI_CHAR, rank-1, 0, comm_cart, MPI_STATUS_IGNORE);
        if(rank < size-1)
            MPI_Send(buffer, BUFFER_SIZE, MPI_CHAR, dest, 0, comm_cart);
        else
            MPI_Send(buffer, BUFFER_SIZE, MPI_CHAR, 0, 0, comm_cart);
    }
    /* Каждая ветвь печатает свой ранг (он же и был послан соседней ветви с большим рангом и значение
    переменной B (ранг соседней ветви с меньшим рангом). */
    printf("rank=%d Buffer=%s\n", rank, buffer);
    fflush(stdout);
    /* Все ветви завершают системные процессы, связанные с топологией comm_cart и завершают
    выполнение программы */
    MPI_Comm_free(&comm_cart);
    MPI_Finalize();
    return 0;
}