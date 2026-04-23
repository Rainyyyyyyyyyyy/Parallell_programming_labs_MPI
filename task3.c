/*
Задание 3: На процессоре задана переменная n = rank.
	Каждый процессор пересылает эту переменную следующему процессору
	(процессор с номером rank = size-1 пересылает процессору с 
	номером rank = 0). На каждом процессоре вывести его номер и переменную n.
*/
/*
*   MPI_Send(&a, 1, MPI_INT, 1%size, tag, MPI_COMM_WORLD); 
            что, сколько_раз, что_тип, на какой проц, тэг(канал)
MPI_Recv(&a, 1, MPI_INT, rank-1, tag, MPI_COMM_WORLD, &status);
        что, сколько_раз, что-тип, от-какого_проца, тэг(канал) 
*/

#include <stdio.h>
#include <mpi.h>


int main(int argc, char **argv) 
{ 
    int rank, size, tag; 
    MPI_Status status; 
    MPI_Init(&argc, &argv); 
    MPI_Comm_size(MPI_COMM_WORLD, &size); 
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    tag = 8;
    
    int n = rank;
    int in_mess = 0;
    MPI_Send(&n, 1, MPI_INT, (n+1)%size, tag, MPI_COMM_WORLD);
    MPI_Recv(&in_mess, 1, MPI_INT, (rank == 0)? size - 1 : rank-1, tag, MPI_COMM_WORLD, &status);

    printf("Proc. №%d got %d\n", rank, in_mess);
    

    MPI_Finalize();
    return 0;
}
