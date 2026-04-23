#include <stdio.h>
#include <mpi.h>
#include <malloc.h>
#include <time.h>
#include <stdlib.h>

/*
   1. На процессе с номером один заполнить массив x_global[]
         случайными числами из диапазона от 0 до 9.
         Размер массива задать равным n = m * size, где m=5,
         а size число используемых процессов.

         Используя коллективные операции библиотеки MPI
         распределить элементы массива x_global[] по процессам.
         Вычислить сумму всех элементов массива.

         Выдать результат на процессе с номером 0.
*/

/*

mpicc lab2.cpp -o lab2.exe -lm

mpirun -np <number_of_processors> ./lab2.exe
*/
// переделать с учётом отправки строк целиком
int main(int argc, char **argv) 
{ 
    srand(time(NULL));
    int rank, size, tag; 
    MPI_Status status;          // статус принятия\ошибки для Recv()
    MPI_Init(&argc, &argv); 
    MPI_Comm_size(MPI_COMM_WORLD, &size); 
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    tag = 8;

    if(size == 1){
        // частный случай
        printf("Incorrect: <size>: %d\n", size);
        MPI_Finalize();
        return 0;
    }
    int m = 5;
    int n = size * m;
    int *x_local = (int*) malloc(m*sizeof(int));

    int *x_global;
    if(rank==1){        // заполнение массива x_global на 1-м процессоре
        x_global = (int*) malloc(n * sizeof(int));
        for(int i=0; i<n; i++){ x_global[i] = i; printf("%d ", x_global[i]); }
        printf("\n");   // вывод
    }
    // рассылка x_global по m элементов на все size процессоры
    // принимающий указатель - x_local
    // источник рассылки - 1-й процессор
    MPI_Scatter(x_global, m, MPI_INT, x_local, m, MPI_INT, 1, MPI_COMM_WORLD);
    if(rank == 0){
        printf("rank %d got: ", rank);
        for(int i=0; i<m; i++){
            printf("%d ", x_local[i]);
        }
        // MessageLoop только для последовательного вывода
        int mesa = 0;
        MPI_Send(&mesa, 1, MPI_INT, 1, tag, MPI_COMM_WORLD);
        MPI_Recv(&mesa, 1, MPI_INT, size-1, tag, MPI_COMM_WORLD, &status);    
    }else{
        int mesa = 0;
        MPI_Recv(&mesa, 1, MPI_INT, rank-1, tag, MPI_COMM_WORLD, &status);
        printf("\nrank %d got: ", rank);
        for(int i=0; i<m; i++){
            printf("%d ", x_local[i]);
        }
        //printf("\n");
        MPI_Send(&mesa, 1, MPI_INT, (rank+1)%size, tag, MPI_COMM_WORLD);
    }

    int local_summ = 0;     // сумма на текущем процессоре (из m элементов)
    int global_summ = 0;    // сумма всех элементов (вернётся на 0-й процессор)
    for(int i=0; i<m; i++)local_summ = local_summ + x_local[i];
        // приём от всех size процессоров
        // указатель-источник - local_summ
        // указатель-приёмник - global_summ
        // Операция (MPI_IP) - сумма (MPI_SUM)
        // процессор приёмник - 0-й процессор
        MPI_Reduce(&local_summ, &global_summ, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    if(rank==0){
        printf("\nSumm: %d\n", global_summ);
    }

    MPI_Finalize();

    return 0;

}
