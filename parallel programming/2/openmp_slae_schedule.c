#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>


#define N 4000
#define EPSILON 1E-7
#define TAU 1E-5
#define MAX_ITERATION_COUNT 1000000

void FillMatrixA(double* A, int matrix_size) {
    for (int i = 0; i < matrix_size; i++) {
        for (int j = 0; j < matrix_size; ++j) {
            A[i * matrix_size + j] = 1;
        }
        A[i * matrix_size + i] = 2;
    }
}

void FillVectorX(double* x, int vector_size) {
    for (int i = 0; i < vector_size; i++) {
        x[i] = 0;
    }
}

void FillVectorB(double* b, int vector_size) {
    for (int i = 0; i < vector_size; i++) {
        b[i] = N + 1;
    }
}

double CalculateNormSquare(const double* vector, int vector_size) {
    double norm_square = 0.0;
#pragma omp parallel for schedule(runtime) \
                     reduction(+ : norm_square)
    for (int i = 0; i < vector_size; ++i) {
        norm_square += vector[i] * vector[i];
    }
    return norm_square;
}

void CalculateAxb(const double* A, const double* x, const double* b, double* Axb, int size) {
#pragma omp parallel for schedule(runtime)
    for (int i = 0; i < size; ++i) {
        Axb[i] = -b[i];
        for (int j = 0; j < N; ++j) {
            Axb[i] += A[i * N + j] * x[j];
        }
    }
}

void CalculateNextX(const double* Axb, double* x, double tau, int vector_size) {
#pragma omp parallel for schedule(runtime)
    for (int i = 0; i < vector_size; ++i) {
        x[i] -= tau * Axb[i];
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
    int iteration_count;
    double accuracy = EPSILON + 1;
    double b_norm;
    double start_time;
    double end_time;
    double* A = malloc(sizeof(double) * N * N);
    double* x = malloc(sizeof(double) * N);
    double* b = malloc(sizeof(double) * N);
    double* Axb = malloc(sizeof(double) * N);

    FillMatrixA(A, N);
    FillVectorX(x, N);
    FillVectorB(b, N);

    b_norm = sqrt(CalculateNormSquare(b, N));

    start_time = omp_get_wtime();

    for (iteration_count = 0; accuracy > EPSILON && iteration_count < MAX_ITERATION_COUNT; ++iteration_count) {
        CalculateAxb(A, x, b, Axb, N);
        CalculateNextX(Axb, x, TAU, N);
        accuracy = sqrt(CalculateNormSquare(Axb, N)) / b_norm;
    }

    end_time = omp_get_wtime();

    if (iteration_count == MAX_ITERATION_COUNT) {
        printf("Too many iterations\n");
    } else {
        printf("Norm: %lf\n", sqrt(CalculateNormSquare(x, N)));
        printf("Time: %lf sec\n", end_time - start_time);
    printf("Iteration: %d\n", iteration_count);
    }

    free(A);
    free(x);
    free(b);
    free(Axb);

    return 0;
}

