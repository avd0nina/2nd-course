#define _XOPEN_SOURCE 700
#include <math.h>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define N 5000
#define EPSILON 1E-7
#define TAU 1E-5
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

void CopyVector(double* dest, const double* src, int vector_size) { // копирует содержимое одного вектора (src) в другой (dest).
    for (int i = 0; i < vector_size; ++i) { // перебираем все элементы вектора.
        dest[i] = src[i]; // Копируем значение src[i] в dest[i].
    }
}

void CalculateAxb(const double* A_chunk, const double* x_chunk, const double* b_chunk, double* replace_x_chunk,
             double* Axb_chunk, int* line_counts, int* line_offsets, int rank, int size) { // фрагмент матрицы A в текущем процессе, x_chunk — кусок вектора x (часть, принадлежащая этому процессу). b_chunk — кусок вектора b. replace_x_chunk — вспомогательный массив для обмена частями вектора x. Axb_chunk — локальный кусок результирующего вектора Ax−b. line_counts — массив с количеством строк, которыми владеет каждый процесс. line_offsets — массив с индексами, откуда начинаются строки в глобальной матрице. rank — номер текущего процесса. size — общее количество процессов.
    int src_rank = (rank + size - 1) % size; // процесс, от которого будем получать данные.
    int dest_rank = (rank + 1) % size; // процесс, которому будем отправлять данные. Это создаёт кольцевую схему обмена данными, где каждый процесс получает данные от предыдущего и передаёт следующему.
    int current_rank;
    CopyVector(replace_x_chunk, x_chunk, line_counts[rank]); // Копируем локальную часть вектора x в replace_x_chunk, так как его содержимое будет изменяться в процессе обмена.
    for (int i = 0; i < size; ++i) { // Чтобы вычислить всю сумму Ax, каждый процесс по очереди использует части вектора x от всех процессов. Обмен данными происходит size раз, так что каждая часть x передаётся по кольцу.
        current_rank = (rank + i) % size; // На каждой итерации i мы работаем с частью вектора x от процесса current_rank. В начале это локальный x, затем он будет приходить от других процессов.
        for (int j = 0; j < line_counts[rank]; ++j) { // Цикл по строкам матрицы A в текущем процессе.
            if (i == 0) { // В первой итерации (i == 0) инициализируем Axb_chunk как -b_chunk, потому что результат считается как Ax−b.
                Axb_chunk[j] = -b_chunk[j];
            }
            for (int k = 0; k < line_counts[current_rank]; ++k) { // Идём по элементам текущего блока вектора x (replace_x_chunk)
                Axb_chunk[j] += A_chunk[j * N + line_offsets[current_rank] + k] * replace_x_chunk[k]; // Берём соответствующие элементы матрицы A, перемножаем и добавляем в Axb_chunk.
            }
        }
        if (i != size - 1) { // Отправляем текущий replace_x_chunk в dest_rank и получаем replace_x_chunk от src_rank. После этого replace_x_chunk содержит следующую часть вектора x. Повторяем вычисления на следующей итерации цикла i.
            MPI_Sendrecv_replace(replace_x_chunk, line_counts[0], MPI_DOUBLE, dest_rank, rank,
                                 src_rank, src_rank, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
    }
}

void СalculateNextX(const double* Axb_chunk, double* x_chunk, double tau, int chunk_size) {
    for (int i = 0; i < chunk_size; ++i) // цикл по локальным элементам вектора x. Каждый процесс обновляет только свою часть вектора x.
        x_chunk[i] -= tau * Axb_chunk[i]; // Обновляем каждый элемент вектора x по формуле градиентного спуска:
}

int main(int argc, char** argv) {
    int rank; // номер текущего процесса.
    int size; // общее количество MPI-процессов.
    int iteration_count; // счётчик итераций метода.
    double b_chunk_norm; // норма локальной части вектора b.
    double b_norm; // глобальная норма вектора b.
    double x_chunk_norm; // норма локальной части вектора x.
    double x_norm; // глобальная норма вектора x.
    double Axb_chunk_norm_square; //  норма вектора невязки Ax−b (возведённая в квадрат).
    double accuracy = EPSILON + 1; // текущая точность (разница между текущим и точным решением).
    double start_time;
    double end_time;
    int* line_counts; // массив, хранящий количество строк матрицы A, принадлежащих каждому процессу.
    int* line_offsets; // массив сдвигов строк для каждого процесса.
    double* x_chunk; // локальные части векторов x и b, принадлежащие процессу.
    double* b_chunk;
    double* A_chunk; //  локальный фрагмент матрицы A, хранящий выделенные процессу строки.
    double* Axb_chunk; // локальная часть вектора невязки Ax−b.
    double* replace_x_chunk; //  буфер для передачи частей векторa x между процессами.
    
    MPI_Init(&argc, &argv); // инициализация MPI.
    MPI_Comm_size(MPI_COMM_WORLD, &size); // узнаём, сколько всего процессов.
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); // получаем номер текущего процесса

    line_counts = malloc(sizeof(int) * size); // выделяем память для хранения количества строк для каждого процесса.
    line_offsets = malloc(sizeof(int) * size); // массив, содержащий сдвиги строк матрицы A.
    
    SplitMatrixLines(line_counts, line_offsets, N, size); // вычисляет, какие строки достанутся каждому процессу.

    x_chunk = malloc(sizeof(double) * line_counts[rank]); // Выделяем память только для локальных частей данных
    b_chunk = malloc(sizeof(double) * line_counts[rank]);
    A_chunk = malloc(sizeof(double) * line_counts[rank] * N);
    
    FillVectorX(x_chunk, line_counts[rank]); //заполняем локальный кусок вектора x.
    FillVectorB(b_chunk, line_counts[rank]); // заполняем локальный кусок вектора b.
    FillMatrixA(A_chunk, line_counts[rank], N, line_offsets[rank]); // заполняем локальный кусок матрицы A, учитывая сдвиг строк (line_offsets[rank]).

    b_chunk_norm = CalculateNormSquare(b_chunk, line_counts[rank]); // вычисляет квадрат нормы локальной части b.
    MPI_Reduce(&b_chunk_norm, &b_norm, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD); // суммирует все локальные квадраты норм и отправляет сумму процессу 0.
    
    if (rank == 0) { // Если rank == 0, извлекаем квадратный корень (чтобы получить норму вектора b).
        b_norm = sqrt(b_norm);
    }
    
    Axb_chunk = malloc(sizeof(double) * line_counts[rank]); // локальная часть вектора невязки Ax−b.
    replace_x_chunk = malloc(sizeof(double) * line_counts[0]); // вспомогательный буфер для передачи частей вектора x.

    start_time = MPI_Wtime(); // Запоминаем начальное время перед началом вычислений.

    for (iteration_count = 0; accuracy > EPSILON && iteration_count < MAX_ITERATION_COUNT; ++iteration_count) { // Цикл продолжается, пока: accuracy > EPSILON (не достигнута требуемая точность). iteration_count < MAX_ITERATION_COUNT (не превысили максимальное число итераций).
        CalculateAxb(A_chunk, x_chunk, b_chunk, replace_x_chunk, Axb_chunk, line_counts, line_offsets, rank, size); // Вычисляем локальную часть невязки Ax−b.
        СalculateNextX(Axb_chunk, x_chunk, TAU, line_counts[rank]); // Обновляем локальную часть вектора x.
        Axb_chunk_norm_square = CalculateNormSquare(Axb_chunk, line_counts[rank]); // Вычисляем норму невязки локально, затем собираем сумму через MPI_Reduce.
        MPI_Reduce(&Axb_chunk_norm_square, &accuracy, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
        if (rank == 0) { // Главный процесс (rank == 0) извлекает корень и делит на норму b.
            accuracy = sqrt(accuracy) / b_norm;
        }
        MPI_Bcast(&accuracy, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD); // Рассылаем обновлённую точность accuracy всем процессам (MPI_Bcast).
    }

    end_time = MPI_Wtime(); // Фиксируем время завершения.

    x_chunk_norm = CalculateNormSquare(x_chunk, line_counts[rank]); // Вычисляем норму полученного решения.
    MPI_Reduce(&x_chunk_norm, &x_norm, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    if (rank == 0) {
        if (iteration_count == MAX_ITERATION_COUNT) { // Если достигнуто максимальное число итераций, выдаём сообщение.
            fprintf(stderr, "Too many iterations\n");
        }
        else {
            printf("Norm: %lf\n", sqrt(x_norm));
            printf("Time: %lf sec\n", end_time - start_time);
        }
    }

    free(line_counts);
    free(line_offsets);
    free(x_chunk);
    free(b_chunk);
    free(A_chunk);
    free(Axb_chunk);

    MPI_Finalize();

    return 0;
}


