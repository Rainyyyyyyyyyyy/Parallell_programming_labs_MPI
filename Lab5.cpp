/*

mpicc program.c -o messageLoop.exe -lm

mpirun -np <number_of_processors> ./messageLoop.exe
*/

/*
2. Вычисление определенных интегралов.
Написать параллельную MPI программу приближенного вычисления определенного
интеграла
∫[a,b] f(x)dx

с точностью e(epsilon)

Вид обобщенной формулы численного интегрирования
определяется вашим вариантом (мсп, мт, мс). Вид функции  f(x) определяется вариантом
задания (1-19). Для контроля точности приближенного вычисления использовать расчет в
системе MathCad или аналогичной. Оценить ускорение и эффективность параллельной
программы, подготовить отчет.

Variant: 2.10: f(x) = e^(1-x) / (2+sin(1+x^2)); метод Трапеция.
a = 0.4,        b = 1.0

1 - Метод средних прямоугольников (мсп);
2 - Метод трапеция (мт);
3 - Метод Симпсона (мс);

*/

#include <iostream>
#include <mpi.h>

#include <cmath>
#include <functional>


// Подынтегральная функция: f(x) = e^(1-x) / (2+sin(1+x^2))
double f(double x) {
    return exp(1.0-x) / (2+sin(1+x*x));
}

// Функция для вычисления интеграла на [local_a, local_b] с local_n трапециями
double integrate(double local_a, double local_b, int local_n) {
    double h = (local_b - local_a) / local_n;
    double sum = (f(local_a) + f(local_b)) / 2.0;
    
    for (int i = 1; i < local_n; i++) {
        double x = local_a + i * h;
        sum = sum + f(x);
    }
    return sum * h;
}

int main(int argc, char** argv) {

    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Status status;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int tag = 8;
    
    double start_time = 0.0, end_time = 0.0;
    double start_time_local = 0.0, end_time_local = 0.0;
    double elapsed = 0.0, elapsed_local = 0.0;

    MPI_Barrier(MPI_COMM_WORLD);
    start_time_local = MPI_Wtime();

    // границы
    double a = 0.4;
    double b = 1.0;
    int n = 10000000;  // кол-во разбиений

    
    //Разбиваение на все процессоры
    double h = (b - a) / n;               // шаг 
    int local_n = n / size;               // трапеций на процесс
    double local_a = a + rank * local_n * h;
    double local_b = local_a + local_n * h;
    
    // локальный интеграл
    double local_sum = integrate(local_a, local_b, local_n);
    
    // Сумма локальных интегралов
    double total_sum = 0.0;
    MPI_Reduce(&local_sum, &total_sum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);
    end_time_local = MPI_Wtime();
    // messageLoop output
    int messageLoop = 0;
    elapsed_local = end_time_local - start_time_local;
    if(rank == 0){
        std::cout<<"elapsed_local on "<<rank<<'\t'<<elapsed_local<<'\n';
        MPI_Send(&messageLoop, 1, MPI_INT, (rank+1)%size, tag, MPI_COMM_WORLD);
        MPI_Recv(&messageLoop, 1, MPI_INT, size-1, tag, MPI_COMM_WORLD, &status);
    }else{
        MPI_Recv(&messageLoop, 1, MPI_INT, rank-1, tag, MPI_COMM_WORLD, &status);
        std::cout<<"elapsed_local on "<<rank<<'\t'<<elapsed_local<<'\n';
        MPI_Send(&messageLoop, 1, MPI_INT, (rank+1)%size, tag, MPI_COMM_WORLD);
    }

    MPI_Reduce(&elapsed_local, &elapsed, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    elapsed = elapsed / size;
    // Процесс 0 выводит результат
    if (rank == 0) {
        std::cout << "Integral from " << a << " to " << b << " = " << total_sum << '\n';
        std::cout<<"time: "<<elapsed<<'\n';
    }
    
    MPI_Finalize();
    return 0;
}
