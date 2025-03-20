#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

#define N 5000
#define EPSILON 1E-7
#define TAU 1E-5
#define MAX_ITERATION_COUNT 1000000

void SplitMatrixLines(int* line_counts, int* line_offsets, int matrix_size, int thread_count) { // Все вычисления происходят в одной параллельной секции, и потоки не создаются заново на каждой итерации. Функция SplitMatrixLines вручную распределяет строки матрицы между потоками, так как OpenMP больше не управляет циклами автоматически. line_counts – массив, сколько строк получает каждый поток. line_offsets – массив, с какой строки начинается область каждого потока. matrix_size – размер матрицы (N). thread_count – количество потоков.
    int offset = 0; // Временная переменная, отслеживающая начало блока строк.
    for (int i = 0; i < thread_count; ++i) { // Цикл распределяет строки между thread_count потоками.
        line_counts[i] = matrix_size / thread_count; // Каждый поток получает N / thread_count строк.
        if (i < matrix_size % thread_count) { // Если N не делится на thread_count без остатка, первые matrix_size % thread_count потоков получают по одной дополнительной строке.
            ++line_counts[i];
        }
        line_offsets[i] = offset; // line_offsets[i] хранит индекс первой строки, которую получает поток i.
        offset += line_counts[i]; // Обновляет offset, чтобы следующий поток начинал с правильной строки.
    }
}

void FillMatrixA(double* A, int matrix_size) { // заполняет матрицу A, double* A – указатель на массив (матрицу). matrix_size – размерность квадратной матрицы (N x N).
    for (int i = 0; i < matrix_size; i++) { // Цикл по строкам матрицы A. i – индекс строки (от 0 до matrix_size - 1).
        for (int j = 0; j < matrix_size; ++j) { // Заполняет строку i матрицы A единицами (1). A[i * matrix_size + j] – доступ к элементу A[i][j] в одномерном массиве, представляющем матрицу.
            A[i * matrix_size + j] = 1;
        }
        A[i * matrix_size + i] = 2; // Заменяет диагональный элемент A[i][i] на 2. A[i * matrix_size + i] – это элемент на главной диагонали.
    }
}

void FillVectorX(double* x, int vector_size) { // заполняет вектор x. double* x – указатель на массив (x – вектор). int vector_size – размерность вектора (количество элементов).
    for (int i = 0; i < vector_size; i++) { // Цикл обходит все элементы вектора x. Каждый элемент x[i] устанавливается в 0.
        x[i] = 0;
    }
}

void FillVectorB(double* b, int vector_size) { // заполняет вектор b. double* b – указатель на массив (b – вектор). int vector_size – размерность вектора.
    for (int i = 0; i < vector_size; i++) { // Заполняет все элементы вектора b значением N + 1. N определено как 5000, значит b[i] = 5001.
        b[i] = N + 1;
    }
}

double CalculateNormSquare(const double* vector, int vector_size) { // вычисляет квадрат нормы вектора. const double* vector – указатель на массив (vector – вектор, который передаётся по ссылке, но не изменяется). int vector_size – размер вектора (количество элементов).
    double norm_square = 0.0; // в нее будет накапливаться сумма квадратов элементов вектора.
    for (int i = 0; i < vector_size; ++i) { // Цикл обходит все элементы вектора (от 0 до vector_size - 1).
        norm_square += vector[i] * vector[i]; // Возводит vector[i] в квадрат. Прибавляет результат к norm_square.
    }
    return norm_square;
}

void CalculateAxb(const double* A, const double* x, const double* b, double* Axb, int size) { // вычисляет разность векторов Ax - b. const double* A – указатель на матрицу A (размер N x N), передаётся как одномерный массив. const double* x – указатель на вектор x (размер N), передаётся как массив. const double* b – указатель на вектор b (размер N). double* Axb – указатель на вектор Axb, в который будет записан результат A * x - b. int size – размерность векторов (по сути, N).
#pragma omp for
    for (int i = 0; i < size; ++i) { // Цикл обходит все строки матрицы A (индекс i). Каждая строка A[i] используется для вычисления соответствующего элемента вектора Axb[i].
        Axb[i] = -b[i]; // Инициализирует Axb[i] значением -b[i], чтобы затем прибавить A[i] * x.
        for (int j = 0; j < N; ++j) { // Внутренний цикл обходит все элементы строки A[i]. j – индекс столбца.
            Axb[i] += A[i * N + j] * x[j]; // Вычисляет скалярное произведение строки A[i] на вектор x.
        }
    }
}

void CalculateNextX(const double* Axb, double* x, double tau, int vector_size) { // // обновляет значения вектора x. const double* Axb – указатель на вектор Axb (размер vector_size), который был вычислен ранее (A * x - b). double* x – указатель на вектор x, который обновляется. double tau – шаг обновления. int vector_size – размер векторов (x и Axb).
#pragma omp for
    for (int i = 0; i < vector_size; ++i) { // Проходит по каждому элементу векторов x и Axb. Каждая итерация обновляет один элемент x[i].
        x[i] -= tau * Axb[i]; //Обновляет x[i] по формуле
    }
}

// void PrintMatrix(const double* a, int lines, int columns) {
//     for (int i = 0; i < lines; i++) {
//         for (int j = 0; j < columns; j++) {
//             printf("%lf ", a[i * columns + j]);
//         }
//         printf("\n");
//     }
// }

int main(int argc, char **argv) {
    int iteration_count = 0; //  счётчик итераций
    int thread_id; // идентификатор потока.
    int thread_count = omp_get_max_threads(); // количество потоков.
    double accuracy = EPSILON + 1; // инициализируем начальную точность.
    double b_norm;
    double start_time;
    double end_time;
    int* line_counts = malloc(sizeof(int) * thread_count); // сколько строк получает каждый поток.
    int* line_offsets = malloc(sizeof(int) * thread_count); // индекс первой строки каждого потока.
    double* A = malloc(sizeof(double) * N * N);
    double* x = malloc(sizeof(double) * N);
    double* b = malloc(sizeof(double) * N);
    double* Axb = malloc(sizeof(double) * N);

    SplitMatrixLines(line_counts, line_offsets, N, thread_count); // разделить строки матрицы между потоками.
    
    FillMatrixA(A, N);
    FillVectorX(x, N);
    FillVectorB(b, N);

    b_norm = sqrt(CalculateNormSquare(b, N));

    start_time = omp_get_wtime(); // Запоминает начальное время выполнения.

#pragma omp parallel private(thread_id) { // Создаёт одну общую параллельную секцию. private(thread_id) – каждый поток получает свою копию переменной thread_id.
        thread_id = omp_get_thread_num(); // Получает идентификатор потока (от 0 до thread_count - 1).
        for (iteration_count = 0; accuracy > EPSILON && iteration_count < MAX_ITERATION_COUNT; ++iteration_count) { // Выполняет итерационный процесс до тех пор, пока точность не станет достаточной.
            CalculateAxb(A + line_offsets[thread_id] * N, x, b + line_offsets[thread_id], // Вычисляет Ax - b, но только для строк, принадлежащих потоку.
                     Axb + line_offsets[thread_id], line_counts[thread_id]);
#pragma omp barrier // Ожидает завершения всех потоков, чтобы не было конфликтов в x.

            CalculateNextX(Axb + line_offsets[thread_id], x + line_offsets[thread_id], // Обновляет x для своего блока строк.
                        TAU, line_counts[thread_id]);

#pragma omp single // обнуляет accuracy, но только в одном потоке.
            accuracy = 0;
#pragma omp atomic // прибавляет CalculateNormSquare в accuracy без конфликтов
            accuracy += CalculateNormSquare(Axb + line_offsets[thread_id], line_counts[thread_id]);
#pragma omp barrier

#pragma omp single // Один поток вычисляет окончательную точность.
            accuracy = sqrt(accuracy) / b_norm;
        }
    }

    end_time = omp_get_wtime(); // Засекает время окончания работы.

    if (iteration_count == MAX_ITERATION_COUNT) {
        printf("Too many iterations\n");
    } else {
        printf("Norm: %lf\n", sqrt(CalculateNormSquare(x, N)));
        printf("Time: %lf sec\n", end_time - start_time);
    }

    free(A);
    free(x);
    free(b);
    free(Axb);

    return 0;
}
