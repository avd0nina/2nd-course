#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

#define N 5000 // размерность системы (число уравнений и неизвестных)
#define EPSILON 1E-7 // точность остановки
#define TAU 1E-5 //  шаг метода
#define MAX_ITERATION_COUNT 100000

void SplitMatrixLines(int* line_counts, int* line_offsets, int matrix_size, int size) { // разделить строки матрицы A между size процессами так, чтобы все процессы получили примерно одинаковое число строк. line_counts — массив, куда записывается, сколько строк получает каждый процесс. line_offsets - массив, куда записывается, с какой строки (смещение) начинается часть матрицы для каждого процесса. matrix_size - общее количество строк в матрице. size — общее число MPI-процессов.
    int offset = 0; // offset (смещение) хранит индекс первой строки, которая назначена текущему процессу. В начале offset = 0, так как первый процесс начинает с 0-й строки.
    for (int i = 0; i < size; ++i) { // Проходим по всем процессам
        line_counts[i] = matrix_size / size; // Каждому процессу назначается matrix_size / size строк.
        if (i < matrix_size % size) { // Если matrix_size не делится без остатка на size, то первые matrix_size % size процессов получают на 1 строку больше.
            ++line_counts[i];
        }
        line_offsets[i] = offset; // Запоминаем offset для текущего процесса (i).
        offset += line_counts[i]; // Увеличиваем offset на количество строк line_counts[i], которые только что назначили процессу i. Таким образом, offset теперь указывает на первую строку следующего процесса.
    }
}

void FillMatrixA(double *A_chunk, int line_count, int line_size, int line_offset) { // заполняем элементы главной диагонали матрицы A - 2.0, остальные равны 1.0. A_chunk — указатель на фрагмент матрицы, line_count — количество строк, которые нужно заполнить. line_size — количество элементов в строке. line_offset — смещение
    for (int i = 0; i < line_count; i++) { // внешний цикл по всем строкам (от 0 до line_count - 1).
        for (int j = 0; j < line_size; ++j) { // вложенный цикл по всем элементам строки (от 0 до line_size - 1).
            A_chunk[i * line_size + j] = 1; // устанавливает значение всех элементов текущей строки матрицы на 1. Индекс элемента в матрице вычисляется как i * line_size + j, где i — это номер строки, а j — номер элемента в строке.
        }
        A_chunk[i * line_size + line_offset + i] = 2; // устанавливает значение 2 на главной диагонали
    }
}

void FillVectorX(double *x, int vector_size) { // Начальные значения элементов вектора x равны 0.
    for (int i = 0; i < vector_size; i++) {
        x[i] = 0;
    }
}

void FillVectorB(double *b, int vector_size) { // Все элементы вектора b равны N+1
    for (int i = 0; i < vector_size; i++) {
        b[i] = N + 1;
    }
}

double CalculateNormSquare(const double *vector, int vector_size) { // вычисляет квадрат нормы вектора.
    double norm_square = 0.0; // Эта переменная будет хранить сумму квадратов элементов вектора, которая и будет квадратом нормы.
    for (int i = 0; i < vector_size; ++i) { // цикл по всем элементам вектора
        norm_square += vector[i] * vector[i]; //  для каждого элемента вектора (с индексом i) вычисляется его квадрат vector[i] * vector[i], и этот квадрат добавляется к переменной norm_square.
    }
    return norm_square;
}

void CalculateAxb(const double* A_chunk, const double* x, const double* b, double* Axb_chunk, int chunk_size, int chunk_offset) { // вычисляет выражение Ax−b для части матрицы A_chunk и вектора x. A_chunk — указатель на часть матрицы A, Axb_chunk — указатель на массив, в который будет записан результат вычисления Ax−b. chunk_size — количество строк в A_chunk, chunk_offset — смещение, используемое для корректного доступа к b
    for (int i = 0; i < chunk_size; ++i) { // внешний цикл по i, который пробегает по строкам части матрицы A_chunk
        Axb_chunk[i] = -b[chunk_offset + i]; // Инициализируется Axb_chunk[i], устанавливая начальное значение равным -b[chunk_offset + i]. chunk_offset + i используется, потому что b хранится как полный вектор, а A_chunk — это только часть матрицы, относящаяся к некоторым строкам.
        for (int j = 0; j < N; ++j) { // Вложенный цикл по j пробегает по столбцам матрицы A_chunk
            Axb_chunk[i] += A_chunk[i * N + j] * x[j]; // Вычисляется сумма произведений элементов строки A_chunk на соответствующие элементы вектора x. A_chunk[i * N + j] — это доступ к элементу матрицы A_chunk в строке i и столбце j, так как матрица хранится в виде одномерного массива. x[j] — соответствующий элемент вектора x. Результат прибавляется к Axb_chunk[i]
        }
    }
}

void CalculateNextX(const double* Axb_chunk, const double* x, double* x_chunk, double tau, int chunk_size, int chunk_offset) { // обновляет часть вектора x в соответствии с методом простой итерации. Axb_chunk — указатель на часть вектора Ax−b, вычисленного ранее.x — указатель на полный вектор x. x_chunk — указатель на массив, в который будет записан обновленный кусок вектора x. tau — параметр метода. chunk_size — количество элементов в обрабатываемом фрагменте вектора. chunk_offset — смещение, указывающее, какой участок вектора x обрабатывается.
    for (int i = 0; i < chunk_size; ++i) { // цикл по i, который проходит по всем chunk_size элементам x_chunk. chunk_size определяет, сколько элементов вектора x обновляет данный процесс (если используется параллельное вычисление).
        x_chunk[i] = x[chunk_offset + i] - tau * Axb_chunk[i]; // Обновляется значение вектора x_chunk[i] с учетом градиентного шага: x[chunk_offset + i] — текущее значение x в соответствующей позиции. tau * Axb_chunk[i] — градиентный шаг, который корректирует значение x. chunk_offset + i используется, так как x — это полный вектор, а x_chunk — его фрагмент.
    }
}

// void PrintMatrix(const double *a, int lines, int columns) {
//     for (int i = 0; i < lines; i++) {
//         for (int j = 0; j < columns; j++) {
//             printf("%lf ", a[i * columns + j]);
//         }
//         printf("\n");
//     }
// }

int main(int argc, char **argv) {
    int rank; // номер текущего процесса в MPI.
    int size; // общее количество процессов.
    int iteration_count; // счётчик итераций метода.
    double b_norm; // норма вектора b.
    double accuracy = EPSILON + 1; // текущее значение критерия остановки (инициализируется значением больше EPSILON).
    double start_time;
    double end_time;
    int* line_counts; //  массив, хранящий количество строк
    int* line_offsets; // массив, хранящий смещение строк
    double* A_chunk; //  локальная подматрица A, обрабатываемая текущим процессом.
    double* x;
    double* b;
    double* Axb_chunk; // временный массив для локальных вычислений.
    double* x_chunk; // временный массив для локальных вычислений.

    MPI_Init(&argc, &argv); // инициализирует среду MPI.
    MPI_Comm_size(MPI_COMM_WORLD, &size); // определяет количество процессов в MPI_COMM_WORLD, записывая его в size.
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); // определяет номер текущего процесса (rank).
    
    line_counts = malloc(sizeof(int) * size); // Выделяется память для массивов line_counts и line_offsets, которые будут хранить, сколько строк получает каждый процесс и с какого индекса они начинаются.
    line_offsets = malloc(sizeof(int) * size);
    
    SplitMatrixLines(line_counts, line_offsets, N, size); // заполняет line_counts и line_offsets, определяя разбиение матрицы A между процессами.

    A_chunk = malloc(sizeof(double) * line_counts[rank] * N); // Выделяется память под локальную подматрицу A_chunk и полные векторы x и b.
    x = malloc(sizeof(double) * N);
    b = malloc(sizeof(double) * N);
    
    FillMatrixA(A_chunk, line_counts[rank], N, line_offsets[rank]); // Заполняются: A_chunk — локальный фрагмент матрицы A. x — начальный вектор x. b — вектор b.
    FillVectorX(x, N);
    FillVectorB(b, N);

    if (rank == 0) { // Если rank == 0, вычисляется евклидова норма вектора b (b_norm), используемая в критерии остановки.
        b_norm = sqrt(CalculateNormSquare(b, N));
    }
    Axb_chunk = malloc(sizeof(double) * line_counts[rank]); // Выделяется память под Axb_chunk (вычисляемое A*x - b) и x_chunk (часть обновлённого x).
    x_chunk = malloc(sizeof(double) * line_counts[rank]);
    
    start_time = MPI_Wtime(); // MPI_Wtime возвращает текущее время, чтобы затем измерить общее время выполнения.

    for (iteration_count = 0; accuracy > EPSILON && iteration_count < MAX_ITERATION_COUNT; ++iteration_count) { // Основной цикл метода градиентного спуска. Условие выхода: accuracy > EPSILON — пока ошибка больше заданного предела точности. iteration_count < MAX_ITERATION_COUNT — пока число итераций не превысило допустимый предел.
        CalculateAxb(A_chunk, x, b, Axb_chunk, line_counts[rank], line_offsets[rank]); // Вычисляется A*x - b для текущего блока данных.
        CalculateNextX(Axb_chunk, x, x_chunk, TAU, line_counts[rank], line_offsets[rank]); // Обновляется часть вектора x для данного процесса.
        MPI_Allgatherv(x_chunk, line_counts[rank], MPI_DOUBLE,
                       x, line_counts, line_offsets, MPI_DOUBLE, MPI_COMM_WORLD); // MPI_Allgatherv собирает обновлённые части x_chunk от всех процессов в x. Это необходимо, потому что x используется всеми процессами.
        double Axb_chunk_norm_square = CalculateNormSquare(Axb_chunk, line_counts[rank]); // Вычисляется квадрат нормы Axb_chunk в локальном процессе.
        MPI_Reduce(&Axb_chunk_norm_square, &accuracy, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD); // MPI_Reduce суммирует Axb_chunk_norm_square со всех процессов и передаёт результат rank == 0.
        if (rank == 0) {
            accuracy = sqrt(accuracy) / b_norm; // Главный процесс (rank == 0) вычисляет относительную погрешность accuracy.
        }
        MPI_Bcast(&accuracy, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD); // MPI_Bcast передаёт accuracy всем процессам.
    }
    end_time = MPI_Wtime(); // Записываем время окончания выполнения алгоритма.
    if (rank == 0) { // Если главный процесс (rank == 0): Проверяется, достигнут ли лимит итераций. Если алгоритм сошелся, выводится норма x и время выполнения.
        if (iteration_count == MAX_ITERATION_COUNT) {
            printf("Too many iterations\n");
        }
        else {
            printf("Norm: %lf\n", sqrt(CalculateNormSquare(x, N)));
            printf("Time: %lf sec\n", end_time - start_time);
        }
    }

    free(line_counts);
    free(line_offsets);
    free(x);
    free(b);
    free(A_chunk);
    free(Axb_chunk);
    free(x_chunk);

    MPI_Finalize();

    return 0;
}

