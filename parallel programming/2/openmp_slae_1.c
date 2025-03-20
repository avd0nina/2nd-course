#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

#define N 5000
#define EPSILON 1E-7
#define TAU 1E-5
#define MAX_ITERATION_COUNT 1000000

void FillMatrixA(double* A, int matrix_size) { // заполняет матрицу A, double* A – указатель на массив (матрицу). matrix_size – размерность квадратной матрицы (N x N).
#pragma omp parallel for // Распараллеливает цикл for (int i = 0; i < matrix_size; i++). OpenMP создаёт несколько потоков, которые выполняют разные итерации i одновременно. Это ускоряет выполнение, так как несколько строк матрицы заполняются параллельно.
    for (int i = 0; i < matrix_size; i++) { // Цикл по строкам матрицы A. i – индекс строки (от 0 до matrix_size - 1).
        for (int j = 0; j < matrix_size; ++j) { // Заполняет строку i матрицы A единицами (1). A[i * matrix_size + j] – доступ к элементу A[i][j] в одномерном массиве, представляющем матрицу.
            A[i * matrix_size + j] = 1;
        }
        A[i * matrix_size + i] = 2; // Заменяет диагональный элемент A[i][i] на 2. A[i * matrix_size + i] – это элемент на главной диагонали.
    }
}

void FillVectorX(double* x, int vector_size) { // заполняет вектор x. double* x – указатель на массив (x – вектор). int vector_size – размерность вектора (количество элементов).
#pragma omp parallel for // Распараллеливает цикл
    for (int i = 0; i < vector_size; i++) { // Цикл обходит все элементы вектора x. Каждый элемент x[i] устанавливается в 0.
        x[i] = 0;
    }
}

void FillVectorB(double* b, int vector_size) { // заполняет вектор b. double* b – указатель на массив (b – вектор). int vector_size – размерность вектора.
#pragma omp parallel for // Распараллеливает цикл
    for (int i = 0; i < vector_size; i++) { // Заполняет все элементы вектора b значением N + 1. N определено как 5000, значит b[i] = 5001.
        b[i] = N + 1;
    }
}

double CalculateNormSquare(const double* vector, int vector_size) { //  вычисляет квадрат нормы вектора. const double* vector – указатель на массив (vector – вектор, который передаётся по ссылке, но не изменяется). int vector_size – размер вектора (количество элементов).
    double norm_square = 0.0; // в нее будет накапливаться сумма квадратов элементов вектора.
#pragma omp parallel for \ // Распараллеливает цикл
                     reduction(+ : norm_square) // – обеспечивает корректное суммирование переменной norm_square в нескольких потоках: Каждый поток вычисляет свою часть суммы. В конце все частичные суммы объединяются в одну переменную norm_square. Без неё потоки могли бы перезаписывать norm_square одновременно, вызывая гонку данных (data race).
    for (int i = 0; i < vector_size; ++i) { // Цикл обходит все элементы вектора (от 0 до vector_size - 1). Каждую итерацию выполняет один из потоков (распределяется OpenMP).
        norm_square += vector[i] * vector[i]; // Возводит vector[i] в квадрат. Прибавляет результат к norm_square.
    }
    return norm_square;
}

void CalculateAxb(const double* A, const double* x, const double* b, double* Axb, int size) { // вычисляет разность векторов Ax - b. const double* A – указатель на матрицу A (размер N x N), передаётся как одномерный массив. const double* x – указатель на вектор x (размер N), передаётся как массив. const double* b – указатель на вектор b (размер N). double* Axb – указатель на вектор Axb, в который будет записан результат A * x - b. int size – размерность векторов (по сути, N).
#pragma omp parallel for // Распараллеливает внешний цикл
    for (int i = 0; i < size; ++i) { // Цикл обходит все строки матрицы A (индекс i). Каждая строка A[i] используется для вычисления соответствующего элемента вектора Axb[i].
        Axb[i] = -b[i]; // Инициализирует Axb[i] значением -b[i], чтобы затем прибавить A[i] * x.
        for (int j = 0; j < N; ++j) { // Внутренний цикл обходит все элементы строки A[i]. j – индекс столбца.
            Axb[i] += A[i * N + j] * x[j]; // Вычисляет скалярное произведение строки A[i] на вектор x.
        }
    }
}

void CalculateNextX(const double* Axb, double* x, double tau, int vector_size) { // обновляет значения вектора x. const double* Axb – указатель на вектор Axb (размер vector_size), который был вычислен ранее (A * x - b). double* x – указатель на вектор x, который обновляется. double tau – шаг обновления. int vector_size – размер векторов (x и Axb).
#pragma omp parallel for // Распараллеливает цикл
    for (int i = 0; i < vector_size; ++i) { // Проходит по каждому элементу векторов x и Axb. Каждая итерация обновляет один элемент x[i].
        x[i] -= tau * Axb[i]; // Обновляет x[i] по формуле
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
    int iteration_count; // счётчик итераций метода
    double accuracy = EPSILON + 1; // точность решения (изначально больше EPSILON, чтобы цикл начался).
    double b_norm; // норма вектора b
    double start_time;
    double end_time;
    double* A = malloc(sizeof(double) * N * N);
    double* x = malloc(sizeof(double) * N);
    double* b = malloc(sizeof(double) * N);
    double* Axb = malloc(sizeof(double) * N);
    
    FillMatrixA(A, N); // заполняет матрицу A (2 на диагонали, 1 в остальных местах).
    FillVectorX(x, N); // заполняет вектор x нулями.
    FillVectorB(b, N); // заполняет вектор b значением N + 1.
    b_norm = sqrt(CalculateNormSquare(b, N)); // вычисляет квадрат нормы, затем sqrt(...) извлекает корень.
    start_time = omp_get_wtime(); // Фиксирует начальное время выполнения алгоритма.
    for (iteration_count = 0; accuracy > EPSILON && iteration_count < MAX_ITERATION_COUNT; ++iteration_count) { // Запускает итерационный процесс, пока: accuracy > EPSILON (погрешность ещё велика). iteration_count < MAX_ITERATION_COUNT (ограничение на число итераций).
        CalculateAxb(A, x, b, Axb, N); // Вычисляет разницу Ax - b и сохраняет её в Axb
        CalculateNextX(Axb, x, TAU, N); // Обновляет вектор x
        accuracy = sqrt(CalculateNormSquare(Axb, N)) / b_norm; // Пересчитывает текущую точность (accuracy). Если accuracy достаточно мала, итерации останавливаются.
    }

    end_time = omp_get_wtime(); // Фиксирует время окончания работы алгоритма

    if (iteration_count == MAX_ITERATION_COUNT) // Если достигнут лимит MAX_ITERATION_COUNT, выводит предупреждение "Слишком много итераций".
        printf("Too many iterations\n");
    else { // Если алгоритм сошёлся, выводит: Норму x, чтобы оценить результат. Время выполнения.
        printf("Norm: %lf\n", sqrt(CalculateNormSquare(x, N)));
        printf("Time: %lf sec\n", end_time - start_time);
    }

    free(A);
    free(x);
    free(b);
    free(Axb);

    return 0;
}
