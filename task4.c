/*Задание 4: На каждом процессоре необходимо создать массив размером size.
	Каждый процессор должен отправить свой номер каждому другому процессору
	для заполнения массивов. Каждый процессор должен заполнить свой массив
	номерами других процессоров. На каждом процессоре вывести его номер
	и полученный массив.
*/
/*
*   MPI_Send(&a, 1, MPI_INT, 1%size, tag, MPI_COMM_WORLD); 
            что, сколько_раз, что_тип, на какой проц, тэг(канал)
MPI_Recv(&a, 1, MPI_INT, rank-1, tag, MPI_COMM_WORLD, &status);
        что, сколько_раз, что-тип, от-какого_проца, тэг(канал) 
*/

#include <stdio.h>
#include <mpi.h>
#include <malloc.h>

int main(int argc, char **argv) 
{ 
    int rank, size, tag; 
    MPI_Status status; 
    MPI_Init(&argc, &argv); 
    MPI_Comm_size(MPI_COMM_WORLD, &size); 
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    tag = 8;

    int mess = 0;   // message
    int *a = malloc(size * 4);
    mess = rank;
    //a[rank] = rank*10;
    a[rank] = -1; //rank;
    for(int i=0; i<rank; i++){
        MPI_Send(&mess, 1, MPI_INT, i, tag, MPI_COMM_WORLD);
    }
    for(int i=rank+1; i<size; i++){
        MPI_Send(&mess, 1, MPI_INT, i, tag, MPI_COMM_WORLD);
    }

    for(int i=0; i<rank; i++){
        mess = 0;
        MPI_Recv(&mess, 1, MPI_INT, i, tag, MPI_COMM_WORLD, &status);
        a[i] = mess;
    }
    for(int i=rank+1; i<size; i++){
        mess = 0;
        MPI_Recv(&mess, 1, MPI_INT, i, tag, MPI_COMM_WORLD, &status);
        a[i] = mess;
    }

    printf("Proc. №%d has [", rank);
    for(int i=0; i<size; i++){
        printf(" %d ", a[i]);
    }
    printf("]\n");

    MPI_Finalize();
    return 0;
}
