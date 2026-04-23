

/*
7. Реализовать параллельный алгоритм вычисления суммы матриц A[][] и B[][].
	* Матрицы заполнить случайными числами из диапазона от -9 до 9. Размер матрицы 
	* задать равным n = 4 * size, m=4, где size число используемых процессов. Матрицы 
	* распределить по процессам по строкам.
    *
	* Результат получить на каждом процессе. 
*/

#include <iostream>
#include <mpi.h>

#include <stdio.h>
#include <time.h>
#include <stdlib.h>


int main(int argc, char **argv){
    
    srand(time(NULL));
    int rank, size, tag;
    MPI_Status status;          // статус принятия\ошибки для Recv()
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    tag = 8;

    /* Стратегия: 
    * На процессоре 0 хранятся обе матрицы A и В
    * с помощью MPI_Scatter разбить матрицы на процессоры по строкам
    * сложить в процессорах строки
    * Разослать результат через MPI_Allgather
    * */
    int *A; // матрицы будут храниться линейно
    int *A_local;   // присылаемые строки матриц 
    int *B;
    int *B_local;

    int *C;         // результирующая матрица: C[] = A[] + B[]
    int *C_local;   // строки матрицы С

    int m = 4;
    int n = size*m;

    if(rank == 0){
        std::cout<<"Matrix A: \t\t\t\t Matrix B: \n";
        /* Заполнение матрицы на 0-м процессоре */
        A = new int[n*m];
        B = new int[n*m];
        for(int i=0; i<n*m; i++){
            A[i] = rand()%19 - 9;
            B[i] = rand()%19 - 9;
        }
        /* Вывод */
        for(int i=0; i<n; i++){
            for(int j=0; j<m; j++){
                std::cout<<A[i*m + j]<<'\t';
            }
            std::cout<<'\t';
            for(int j=0; j<m; j++){
                std::cout<<B[i*m + j]<<'\t';
            }
            std::cout<<'\n';
        }
    }
    A_local = new int[m*m];
    B_local = new int[m*m];
    //for(int i=0; i<m; i++)A_local[i] = B_local[i] = -10;    // граничное значение (для теста Scatter)
    MPI_Scatter(A, m*m, MPI_INT, A_local, m*m, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Scatter(B, m*m, MPI_INT, B_local, m*m, MPI_INT, 0, MPI_COMM_WORLD);
    
    

    int mesa = 0;   // для Message Loop;
    if(rank == 0){
        std::cout<<"=============proc № 0 got: (A_local): \n";
        for(int i=0; i<m; i++){
            for(int j=0; j<m; j++){
                std::cout<<A_local[i*m + j]<<'\t';
            }
            std::cout<<'\n';
        }
        std::cout<<"\t(B_local): \n";
        for(int i=0; i<m; i++){
            for(int j=0; j<m; j++){
                std::cout<<B_local[i*m+j]<<'\t';
            }
            std::cout<<'\n';
        }
        std::cout<<'\n';
        MPI_Send(&mesa, 1, MPI_INT, (rank+1)%size, tag, MPI_COMM_WORLD);
        MPI_Recv(&mesa, 1, MPI_INT, size-1, tag, MPI_COMM_WORLD, &status); 
    }else{
        MPI_Recv(&mesa, 1, MPI_INT, rank-1, tag, MPI_COMM_WORLD, &status); 
        std::cout<<"=============proc № "<<rank<<" got: (A_local): \n";
        for(int i=0; i<m; i++){
            for(int j=0; j<m; j++){
                std::cout<<A_local[i*m+j]<<'\t';
            }
            std::cout<<'\n';
        }
        std::cout<<"\t(B_local): \n";
        for(int i=0; i<m; i++){
            for(int j=0; j<m; j++){
                std::cout<<B_local[i*m+j]<<'\t';
            }
            std::cout<<'\n';
        }
        std::cout<<'\n';
        int mesa = 0;
        MPI_Send(&mesa, 1, MPI_INT, (rank+1)%size, tag, MPI_COMM_WORLD);
    }
    /* Выделение памяти под строки матрицы С*/
    C_local = new int[m*m];
    for(int i=0; i<m*m; i++){
        C_local[i] = A_local[i] + B_local[i];
    }
    /* Выделение памяти под матрицу С */
    //if(rank == 0){
        C = new int[n*m];
    //}

    // MPI_Gather как MPI_Reduce, но без MPI_OP (операции)
    //MPI_Gather(C_local, m*m, MPI_INT, C, m*m, MPI_INT, 0, MPI_COMM_WORLD);
    /* Вывод по принципу MessageLoop */
    MPI_Allgather(C_local, m*m, MPI_INT, C, m*m, MPI_INT, MPI_COMM_WORLD);
    if(rank == 0){  // один раз выводим
        std::cout<<"Result (Matrix C on proc. №"<<rank<<"): \n";
        for(int i=0; i<n; i++){
            for(int j=0; j<m; j++){
                std::cout<<C[i*m + j]<<'\t';
            }
            std::cout<<'\n';
        }
        MPI_Send(&mesa, 1, MPI_INT, (rank+1)%size, tag, MPI_COMM_WORLD);
        MPI_Recv(&mesa, 1, MPI_INT, size-1, tag, MPI_COMM_WORLD, &status); 
    }else{
        MPI_Recv(&mesa, 1, MPI_INT, rank-1, tag, MPI_COMM_WORLD, &status);
        std::cout<<"Result (Matrix C on proc. №"<<rank<<"): \n";
        for(int i=0; i<n; i++){
            for(int j=0; j<m; j++){
                std::cout<<C[i*m + j]<<'\t';
            }
            std::cout<<'\n';
        }
         MPI_Send(&mesa, 1, MPI_INT, (rank+1)%size, tag, MPI_COMM_WORLD);
    }


    delete[] A_local;
    delete[] B_local;
    delete[] C_local;
    delete[] C;
    if(rank == 0){
        delete[] A;
        delete[] B;
        //delete[] C;
    }

    
    MPI_Finalize();
    return 0;
}
