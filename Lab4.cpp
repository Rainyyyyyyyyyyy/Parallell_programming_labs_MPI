#include <mpi.h>
#include <iostream>
#include <cstdlib>
#include <ctime>

void print_first_and_last_N_elems(int * const& array, const int &size, const int &N){
    for(int i=0; i<N; i++){
        std::cout<<array[i]<<' ';
    }
    std::cout<<"... ";
    for(int i=size-1-N; i<size; i++){
        std::cout<<array[i]<<' ';
    }
}


int main(int argc, char** argv){

    srand(time(NULL)); // init for rand()
    int rank, size;
    MPI_Status status;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int tag = 8;

    int *X; 
    int *Y;

    int n = 1000000 + rand()%9000001;
    
    int q = n/size;
    // добавление количества элементов в *X, *Y до кратного size
    // если до этого было кратно, то не добавляем 
    int qn = (n%size == 0)? n : (q+1)*size;
    if(rank == 0){
        std::cout<<"n = "<<n<<'\n';
        X = new int[qn];
        Y = new int[qn];
        for(int i=0; i<n; i++){
            X[i] = Y[i] = 1;
        }
        // добавочные элементы обнуляются (заглушка)
        for(int i=n; i<qn; i++){
            X[i] = Y[i] = 0;
        }
        std::cout<<" X,Y: ";
        print_first_and_last_N_elems(X, qn, size);
        std::cout<<'\n';
    }

    int send_recv_number = qn / size;  

    int *X_local = new int[send_recv_number];
    int *Y_local = new int[send_recv_number];
    MPI_Scatter(X, send_recv_number, MPI_INT, X_local, send_recv_number, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Scatter(Y, send_recv_number, MPI_INT, Y_local, send_recv_number, MPI_INT, 0, MPI_COMM_WORLD);
    


    int sum_local = 0;
    int sum = 0;
    // внутренняя сумма (сумма части из qn / size элементов)
    for(int i=0; i<send_recv_number; i++){
        sum = sum + X_local[i]*Y_local[i];
    }

    int wall = size;    // граница для (сдваивания)
    int middle = wall >> 1; // середина рассматриваемого мн-ва
    while(wall>1){
        // нечётное мн-во
        if(wall & 1){
            // сдваивание двух последних (теперь мн-во снова чётное)
            if(rank == wall-1){
                MPI_Send(&sum, 1, MPI_INT, rank-1, tag, MPI_COMM_WORLD);
            }
            if(rank == wall-2){
                MPI_Recv(&sum_local, 1, MPI_INT, rank+1, tag, MPI_COMM_WORLD, &status);
                sum = sum + sum_local;
            }
            wall--;
        // чётное мн-во (просто сдваиваем )
        }else{
            if(rank >= middle){
                MPI_Send(&sum, 1, MPI_INT, rank - middle, tag, MPI_COMM_WORLD);
            }else{
                MPI_Recv(&sum_local, 1, MPI_INT, rank + middle, tag, MPI_COMM_WORLD, &status);
                sum = sum + sum_local;
            }
        wall = middle;
        middle = wall >> 1;
        }
    }


    // MessageLoop вывод переменной sum на каждом процессоре
    int mess = 0;
    if(rank == 0){
        std::cout<<"Result on proc. №0: \n";
        std::cout<<"rank = "<<rank<<" Sum: "<<sum<<'\n';
        MPI_Send(&mess, 1, MPI_INT, (rank+1)%size, tag, MPI_COMM_WORLD);
        MPI_Recv(&mess, 1, MPI_INT, size-1, tag, MPI_COMM_WORLD, &status);
    }else{
        MPI_Recv(&mess, 1, MPI_INT, rank-1, tag, MPI_COMM_WORLD, &status);
        std::cout<<"rank = "<<rank<<" Sum: "<<sum<<'\n';
        MPI_Send(&mess, 1, MPI_INT, (rank+1)%size, tag, MPI_COMM_WORLD);
    }
    
    if(rank == 0){
        delete[] X;
        delete[] Y;
    }
    delete[] X_local;
    delete[] Y_local;
    MPI_Finalize();
}

