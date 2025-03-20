#define _XOPEN_SOURCE 700
#include <math.h>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

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

void CopyVector(double* dest, const double* src, int vector_size) {
    for (int i = 0; i < vector_size; ++i) {
        dest[i] = src[i];
    }
}

void CalculateAxb(const double* A_chunk, const double* x_chunk, const double* b_chunk, double* replace_x_chunk,
             double* Axb_chunk, int* line_counts, int* line_offsets, int rank, int size) {
    int src_rank = (rank + size - 1) % size;
    int dest_rank = (rank + 1) % size;
    int current_rank;
    CopyVector(replace_x_chunk, x_chunk, line_counts[rank]);
    for (int i = 0; i < size; ++i) {
        current_rank = (rank + i) % size;
        for (int j = 0; j < line_counts[rank]; ++j) {
            if (i == 0) {
                Axb_chunk[j] = -b_chunk[j];
            }
            for (int k = 0; k < line_counts[current_rank]; ++k) {

                Axb_chunk[j] += A_chunk[j * N + line_offsets[current_rank] + k] * replace_x_chunk[k];
            }
        }
        if (i != size - 1) {
            MPI_Sendrecv_replace(replace_x_chunk, line_counts[0], MPI_DOUBLE, dest_rank, rank,
                                 src_rank, src_rank, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
    }
}

void СalculateNextX(const double* Axb_chunk, double* x_chunk, double tau, int chunk_size) {
    for (int i = 0; i < chunk_size; ++i)
        x_chunk[i] -= tau * Axb_chunk[i];
}

int main(int argc, char** argv) {
    int rank;
    int size;
    int iteration_count;
    double b_chunk_norm;
    double b_norm;
    double x_chunk_norm;
    double x_norm;
    double Axb_chunk_norm_square;
    double accuracy = EPSILON + 1;
    double start_time;
    double end_time;
    int* line_counts;
    int* line_offsets;
    double* x_chunk;
    double* b_chunk;
    double* A_chunk;
    double* Axb_chunk;
    double* replace_x_chunk;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    line_counts = malloc(sizeof(int) * size);
    line_offsets = malloc(sizeof(int) * size);
    
    SplitMatrixLines(line_counts, line_offsets, N, size);

    x_chunk = malloc(sizeof(double) * line_counts[rank]);
    b_chunk = malloc(sizeof(double) * line_counts[rank]);
    A_chunk = malloc(sizeof(double) * line_counts[rank] * N);
    
    FillVectorX(x_chunk, line_counts[rank]);
    FillVectorB(b_chunk, line_counts[rank]);
    FillMatrixA(A_chunk, line_counts[rank], N, line_offsets[rank]);


    b_chunk_norm = CalculateNormSquare(b_chunk, line_counts[rank]);
    MPI_Reduce(&b_chunk_norm, &b_norm, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    
    if (rank == 0) {
        b_norm = sqrt(b_norm);
    }
    
    Axb_chunk = malloc(sizeof(double) * line_counts[rank]);
    replace_x_chunk = malloc(sizeof(double) * line_counts[0]);

    start_time = MPI_Wtime();

    for (iteration_count = 0; accuracy > EPSILON && iteration_count < MAX_ITERATION_COUNT; ++iteration_count) {
        CalculateAxb(A_chunk, x_chunk, b_chunk, replace_x_chunk, Axb_chunk, line_counts, line_offsets, rank, size);
        СalculateNextX(Axb_chunk, x_chunk, TAU, line_counts[rank]);
        Axb_chunk_norm_square = CalculateNormSquare(Axb_chunk, line_counts[rank]);
        MPI_Reduce(&Axb_chunk_norm_square, &accuracy, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
        if (rank == 0) {
            accuracy = sqrt(accuracy) / b_norm;
        }
        MPI_Bcast(&accuracy, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    }

    end_time = MPI_Wtime();
    x_chunk_norm = CalculateNormSquare(x_chunk, line_counts[rank]);
    MPI_Reduce(&x_chunk_norm, &x_norm, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    if (rank == 0) {
        if (iteration_count == MAX_ITERATION_COUNT) {
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

