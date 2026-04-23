#include <stdio.h>
#include <mpi.h>
#include <malloc.h>
#include <time.h>
#include <stdlib.h>

/*
    * На процессе номер ноль заполнить матрицу A_global[n][m] случайными числами из 
    * диапазона от -9 до 9. Размер массива задать равным n = (size+1) * size/2, m = 5, где 
    * size число используемых процессов. Разослать матрицу A_global[][] по процессам 
    * следующим образом. На 1-й процесс нулевую строку матрицы A_global[][], на 2-й 
    * процесс первую и вторую строки матрицы A_global[][], и т.д.
*/

/*

mpicc Lab1.c -o Lab1.exe -lm

mpirun -np <number_of_processors> ./Lab1.exe
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

    int n =  ((size+1)*size)>>1;
    int m = 5;
    if(rank == 0){
        /*      заполнение матрицы  */
        int **A_global = (int**)malloc(sizeof(int *) * n);    // Matrix
        for(int i=0; i<n; i++){
            A_global[i] = (int*)malloc(sizeof(int) * m);
            for(int j=0; j<m; j++){
                A_global[i][j] = rand()%19 - 9;    // a[i][j] = {-9, -8, ..., -1, 0, 1, ..., 8, 9}
            }
        }
        printf("\tProc № 0: \n");
        for(int i=0; i<n; i++){
            for(int j=0; j<m; j++){
                printf("\t%d", A_global[i][j]);
            }
            printf("\n");
        }

        /*      Отправка матрицы  */
        int index = 0;
        for(int PROC=1; PROC<size; PROC++){     // на все процессоры
            for(int amount = 0; amount < PROC; amount++){   // кол-во строк
                MPI_Send(A_global[index + amount], m, MPI_INT, PROC, tag, MPI_COMM_WORLD);
            }
            index = index + PROC;
        }
        /*      очистка памяти    */
        //for(int i=0; i<n; i++)free(A_global[i]);
        free(A_global);
    }
    else{
        /*      Получение матрицы  */
        int **a = (int**) malloc(sizeof(int*) * rank);
        for(int PROC=0; PROC<rank; PROC++){
            a[PROC] = (int*) malloc(sizeof(int) * m);
            MPI_Recv(a[PROC], m, MPI_INT, 0, tag, MPI_COMM_WORLD, &status);
            /*for(int j=0; j<m; j++){
                MPI_Recv(&a[i][j], 1, MPI_INT, 0, tag, MPI_COMM_WORLD, &status);
            }*/
        }
        /*      Вывод матрицы  Message_Loop для последовательного вывода */
        int mesa = 0;
        if(rank == 1){
            printf("Proc. №%d got:\t", rank);
            for(int i=0; i<rank; i++){
                printf("[\t");
                for(int j=0; j<m; j++){
                    printf("%d\t", a[i][j]);
                }
                printf("]\n\t\t");
            }
         printf("\n");
         MPI_Send(&mesa, 1, MPI_INT, (rank+1)%size, tag, MPI_COMM_WORLD);
        }else{
            if(rank != 0){
                MPI_Recv(&mesa, 1,  MPI_INT, rank-1, tag, MPI_COMM_WORLD, &status);
                printf("Proc. №%d got:\t", rank);
            for(int i=0; i<rank; i++){
                printf("[\t");
                for(int j=0; j<m; j++){
                    printf("%d\t", a[i][j]);
                }
                printf("]\n\t\t");
            }
         printf("\n");
         MPI_Send(&mesa, 1, MPI_INT, (rank+1)%size, tag, MPI_COMM_WORLD);
            }else{
                MPI_Recv(&mesa, 1, MPI_INT, size-1, tag, MPI_COMM_WORLD, &status);
            }
        }
        // очистка памяти
        //for(int i=0; i<rank; rank++)free(a[i]);
        free(a);
    }


    MPI_Finalize();

    return 0;

}
