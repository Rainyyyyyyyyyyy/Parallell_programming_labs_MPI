/*
Задание 2: На процессоре rank = 0 задан массив размера size,
	заполненный первыми цифрами числа 𝝅. Необходимо
	переслать каждому процессору, начиная с rank = 1,
	число из этого массивa с индексом, соответствующим номеру процессора.
	На каждом процессоре, начиная с rank = 1, вывести его номер и полученное число.
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

#define PI 0.314159265358979323846


int main(int argc, char **argv) 
{ 
    int rank, size, tag; 
    MPI_Status status; 
    MPI_Init(&argc, &argv); 
    MPI_Comm_size(MPI_COMM_WORLD, &size); 
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    tag = 8;
    int in_mess = 0;
    int *a;
    if(rank == 0){
        /* Writing Pi in *a: */
        a = malloc(4*size);
        int mess = 0;
        for(int i=1; i<size; i++){
            double temp = PI;
            for(int j=0; j<i; j++){
                temp = temp * 10;  
            }
            mess = (int)(temp) % 10;
            a[i] = mess;
        }
        /* Sending *a to other procs*/
        for(int i=1; i<size; i++){
            MPI_Send(&a[i], 1, MPI_INT, i, tag, MPI_COMM_WORLD);
        }
        free(a);
    }else{
        /* recieving *a from 0-st proc*/
        MPI_Recv(&in_mess, 1, MPI_INT, 0, tag, MPI_COMM_WORLD, &status);
        printf("Proc. №%d got %d\n", rank, in_mess);
    }
    /* end. */
    MPI_Finalize();

    return 0;
}

