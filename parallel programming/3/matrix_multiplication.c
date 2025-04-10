#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <mpi.h>

#define DIMS_COUNT 2
#define X 0
#define Y 1

void InitializeDims(int dims[DIMS_COUNT], int size, int argc, char **argv) {
    if (argc < 3) {
        MPI_Dims_create(size, DIMS_COUNT, dims);
    } else {
        dims[X] = atoi(argv[1]);
        dims[Y] = atoi(argv[2]);
        if (dims[X] * dims[Y] != size) {
            exit(EXIT_FAILURE);
        }
    }
}

void InitializeCommunicators(const int dims[DIMS_COUNT], MPI_Comm *comm_grid, MPI_Comm *comm_rows, MPI_Comm *comm_columns) {
    int reorder = 1;
    int periods[DIMS_COUNT] = {0, 0};
    int sub_dims[DIMS_COUNT] = {0, 0};
    MPI_Cart_create(MPI_COMM_WORLD, DIMS_COUNT, dims, periods, reorder, comm_grid);
    sub_dims[X] = false;
    sub_dims[Y] = true;
    MPI_Cart_sub(*comm_grid, sub_dims, comm_rows);
    sub_dims[X] = true;
    sub_dims[Y] = false;
    MPI_Cart_sub(*comm_grid, sub_dims, comm_columns);
}

void GenerateMatrix(double *matrix, int column, int leading_row, int leading_column, bool onRows) {
    for (int i = 0; i < leading_row; ++i) {
        for (int j = 0; j < leading_column; ++j) {
            matrix[i * column + j] = onRows ? i : j;
        }
    }
}

void SplitA(const double *A, double *A_block, int A_block_size, int n2, int coords_y, MPI_Comm comm_rows, MPI_Comm comm_columns) {
    if (coords_y == 0) {
        MPI_Scatter(A, A_block_size * n2, MPI_DOUBLE, A_block, A_block_size * n2, MPI_DOUBLE, 0, comm_columns);
    }
    MPI_Bcast(A_block, A_block_size * n2, MPI_DOUBLE, 0, comm_rows);
}

void SplitB(const double *B, double *B_block, int B_block_size, int n2, int aligned_n3, int coords_x, MPI_Comm comm_rows, MPI_Comm comm_columns) {
    if (coords_x == 0) {
        MPI_Datatype column_not_resized_t;
        MPI_Datatype column_resized_t;
        MPI_Type_vector(n2, B_block_size, aligned_n3, MPI_DOUBLE, &column_not_resized_t);
        MPI_Type_commit(&column_not_resized_t);
        MPI_Type_create_resized(column_not_resized_t, 0, B_block_size * sizeof(double), &column_resized_t);
        MPI_Type_commit(&column_resized_t);
        MPI_Scatter(B, 1, column_resized_t, B_block, B_block_size * n2, MPI_DOUBLE, 0, comm_rows);
        MPI_Type_free(&column_not_resized_t);
        MPI_Type_free(&column_resized_t);
    }
    MPI_Bcast(B_block, B_block_size * n2, MPI_DOUBLE, 0, comm_columns);
}

void Multiply(const double *A_block, const double *B_block, double *C_block, int A_block_size, int B_block_size, int n2) {
    for (int i = 0; i < A_block_size; ++i) {
        for (int j = 0; j < B_block_size; ++j) {
            C_block[i * B_block_size + j] = 0;
        }
    }
    for (int i = 0; i < A_block_size; ++i) {
        for (int j = 0; j < n2; ++j) {
            for (int k = 0; k < B_block_size; ++k) {
                C_block[i * B_block_size + k] += A_block[i * n2 + j] * B_block[j * B_block_size + k];
            }
        }
    }
}

void GatherC(const double *C_block, double *C, int A_block_size, int B_block_size, int aligned_n1, int aligned_n3, int size, MPI_Comm comm_grid) {
    MPI_Datatype not_resized_recv_t;
    MPI_Datatype resized_recv_t;
    int dims_x = aligned_n1 / A_block_size;
    int dims_y = aligned_n3 / B_block_size;
    int *recv_counts = malloc(sizeof(int) * size);
    int *displs = malloc(sizeof(int) * size);
    MPI_Type_vector(A_block_size, B_block_size, aligned_n3, MPI_DOUBLE, &not_resized_recv_t);
    MPI_Type_commit(&not_resized_recv_t);
    MPI_Type_create_resized(not_resized_recv_t, 0, B_block_size * sizeof(double), &resized_recv_t);
    MPI_Type_commit(&resized_recv_t);
    for (int i = 0; i < dims_x; ++i) {
        for (int j = 0; j < dims_y; ++j) {
            recv_counts[i * dims_y + j] = 1;
            displs[i * dims_y + j] = j + i * dims_y * A_block_size;
        }
    }
    MPI_Gatherv(C_block, A_block_size * B_block_size, MPI_DOUBLE, C, recv_counts, displs, resized_recv_t, 0, comm_grid);
    MPI_Type_free(&not_resized_recv_t);
    MPI_Type_free(&resized_recv_t);
    free(recv_counts);
    free(displs);
}

bool CheckC(const double *C, int column, int leading_row, int leading_column, int n2) {
    for (int i = 0; i < leading_row; ++i) {
        for (int j = 0; j < leading_column; ++j) {
            if (C[i * column + j] != (double)i * j * n2) {
                printf("(%d, %d)\n", i, j);
                printf("%lf != %lf\n", C[i * column + j], (double)i * j * n2);
                return false;
            }
        }
    }
    return true;
}

int main(int argc, char **argv) {
    int n1 = 2000;
    int n2 = 2200;
    int n3 = 2100;
    int rank;
    int size;
    int aligned_n1;
    int aligned_n3;
    int A_block_size;
    int B_block_size;
    int dims[DIMS_COUNT] = {0, 0};
    int coords[DIMS_COUNT] = {0, 0};
    double start_time;
    double end_time;
    double *A = NULL;
    double *B = NULL;
    double *C = NULL;
    double *A_block = NULL;
    double *B_block = NULL;
    double *C_block = NULL;
    MPI_Comm comm_grid;
    MPI_Comm comm_rows;
    MPI_Comm comm_columns;

    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    InitializeDims(dims, size, argc, argv);
    InitializeCommunicators(dims, &comm_grid, &comm_rows, &comm_columns);

    MPI_Cart_coords(comm_grid, rank, DIMS_COUNT, coords);

    A_block_size = ceil((double)n1 / dims[X]);
    B_block_size = ceil((double)n3 / dims[Y]);
    aligned_n1 = A_block_size * dims[X];
    aligned_n3 = B_block_size * dims[Y];

    if (coords[X] == 0 && coords[Y] == 0) {
        A = malloc(sizeof(double) * aligned_n1 * n2);
        B = malloc(sizeof(double) * n2 * aligned_n3);
        C = malloc(sizeof(double) * aligned_n1 * aligned_n3);
        GenerateMatrix(A, n2, n1, n2, true);
        GenerateMatrix(B, aligned_n3, n2, n3, false);
    }

    start_time = MPI_Wtime();

    A_block = malloc(sizeof(double) * A_block_size * n2);
    B_block = malloc(sizeof(double) * B_block_size * n2);
    C_block = malloc(sizeof(double) * A_block_size * B_block_size);

    SplitA(A, A_block, A_block_size, n2, coords[Y], comm_rows, comm_columns);
    SplitB(B, B_block, B_block_size, n2, aligned_n3, coords[X], comm_rows, comm_columns);

    Multiply(A_block, B_block, C_block, A_block_size, B_block_size, n2);
    GatherC(C_block, C, A_block_size, B_block_size, aligned_n1, aligned_n3, size, comm_grid);

    end_time = MPI_Wtime();

    if (coords[Y] == 0 && coords[X] == 0) {
        printf("Is matrix C correct? - %s\n", CheckC(C, aligned_n3, n1, n3, n2) ? "yes" : "no");
        printf("Time: %lf\n", end_time - start_time);
        free(A);
        free(B);
        free(C);
    }

    free(A_block);
    free(B_block);
    free(C_block);
    MPI_Comm_free(&comm_grid);
    MPI_Comm_free(&comm_rows);
    MPI_Comm_free(&comm_columns);

    MPI_Finalize();

    return EXIT_SUCCESS;
}
