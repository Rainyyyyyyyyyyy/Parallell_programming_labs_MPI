/*
*   MPI_Send(&a, 1, MPI_INT, 1%size, tag, MPI_COMM_WORLD); 
            что, сколько_раз, что_тип, на какой проц, тэг(канал)
MPI_Recv(&a, 1, MPI_INT, rank-1, tag, MPI_COMM_WORLD, &status);
        что, сколько_раз, что-тип, от-какого_проца, тэг(канал) 
*/
#include <stdio.h>
#include <mpi.h>
#include <malloc.h>

/*
Задание 1: На процессоре rank = 0 задан массив размера size.
	Необходимо заполнить этот массив номерами других процессоров.
	Вывести получившийся массив.
*/
int main(int argc, char **argv) 
{ 
    int rank, size, tag; 
    MPI_Status status; 
    MPI_Init(&argc, &argv); 
    MPI_Comm_size(MPI_COMM_WORLD, &size); 
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    tag = 8;

    int mess = 0;   // message
    int *a;
    if(rank==0){
        a = malloc(size*4); // size - байтов, 4 - размер int в байтах
        a[0] = 0;
        for(int i=1; i<size; i++){
            MPI_Recv(&mess, 1, MPI_INT, i, tag, MPI_COMM_WORLD, &status);
            a[i] = mess;
        }
        for(int i=0; i<size; i++){
            printf("%d ", a[i]);
        }
        printf("\n");
        free(a);
    }else{
        MPI_Send(&rank, 1, MPI_INT, 0, tag, MPI_COMM_WORLD);
    }

    MPI_Finalize();




    return 0;
}
