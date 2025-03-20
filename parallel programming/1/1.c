#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

#define N 5000
#define EPSILON 1E-7
#define TAU 1E-5
#define MAX_ITERATION_COUNT 100000

void SplitMatrixLines(int* line_counts, int* line_offsets, int matrix_size, int size) {
    int offset = 0;
    for (int i = 0; i < size; ++i) {
        line_counts[i] = matrix_size / size;
        if (i < matrix_size % size) {
            ++line_counts[i];
        }
        line_offsets[i] = offset;
        offset += line_counts[i];
    }
}

void FillMatrixA(double *A_chunk, int line_count, int line_size, int line_offset) {
    for (int i = 0; i < line_count; i++) {
        for (int j = 0; j < line_size; ++j) {
            A_chunk[i * line_size + j] = 1;
        }
        A_chunk[i * line_size + line_offset + i] = 2;
    }
}

void FillVectorX(double *x, int vector_size) {
    for (int i = 0; i < vector_size; i++) {
        x[i] = 0;
    }
}

void FillVectorB(double *b, int vector_size) {
    for (int i = 0; i < vector_size; i++) {
        b[i] = N + 1;
    }
}

double CalculateNormSquare(const double *vector, int vector_size) {
    double norm_square = 0.0;
    for (int i = 0; i < vector_size; ++i) {
        norm_square += vector[i] * vector[i];
    }
    return norm_square;
}

void CalculateAxb(const double* A_chunk, const double* x, const double* b, double* Axb_chunk, int chunk_size, int chunk_offset) {
    for (int i = 0; i < chunk_size; ++i) {
        Axb_chunk[i] = -b[chunk_offset + i];
        for (int j = 0; j < N; ++j) {
            Axb_chunk[i] += A_chunk[i * N + j] * x[j];
        }
    }
}

void CalculateNextX(const double* Axb_chunk, const double* x, double* x_chunk, double tau, int chunk_size, int chunk_offset) {
    for (int i = 0; i < chunk_size; ++i) {
        x_chunk[i] = x[chunk_offset + i] - tau * Axb_chunk[i];
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
    int rank;
    int size;
    int iteration_count;
    double b_norm;
    double accuracy = EPSILON + 1;
    double start_time;
    double end_time;
    int* line_counts;
    int* line_offsets;
    double* A_chunk;
    double* x;
    double* b;
    double* Axb_chunk;
    double* x_chunk;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    line_counts = malloc(sizeof(int) * size);
    line_offsets = malloc(sizeof(int) * size);
    
    SplitMatrixLines(line_counts, line_offsets, N, size);

    A_chunk = malloc(sizeof(double) * line_counts[rank] * N);
    x = malloc(sizeof(double) * N);
    b = malloc(sizeof(double) * N);
    
    FillMatrixA(A_chunk, line_counts[rank], N, line_offsets[rank]);
    FillVectorX(x, N);
    FillVectorB(b, N);

    if (rank == 0) {
        b_norm = sqrt(CalculateNormSquare(b, N));
    }
    Axb_chunk = malloc(sizeof(double) * line_counts[rank]);
    x_chunk = malloc(sizeof(double) * line_counts[rank]);
    
    start_time = MPI_Wtime();

    for (iteration_count = 0; accuracy > EPSILON && iteration_count < MAX_ITERATION_COUNT; ++iteration_count) {
        CalculateAxb(A_chunk, x, b, Axb_chunk, line_counts[rank], line_offsets[rank]);
        CalculateNextX(Axb_chunk, x, x_chunk, TAU, line_counts[rank], line_offsets[rank]);
        MPI_Allgatherv(x_chunk, line_counts[rank], MPI_DOUBLE,
                       x, line_counts, line_offsets, MPI_DOUBLE, MPI_COMM_WORLD);
        double Axb_chunk_norm_square = CalculateNormSquare(Axb_chunk, line_counts[rank]);

        MPI_Reduce(&Axb_chunk_norm_square, &accuracy, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
        if (rank == 0) {
            accuracy = sqrt(accuracy) / b_norm;
        }
        MPI_Bcast(&accuracy, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    }
    end_time = MPI_Wtime();
    if (rank == 0) {
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

