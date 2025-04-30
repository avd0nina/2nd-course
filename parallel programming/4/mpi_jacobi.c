#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <mpi.h>

#define X_0 (double)-1.0
#define Y_0 (double)-1.0
#define Z_0 (double)-1.0

#define D_X (double)2.0
#define D_Y (double)2.0
#define D_Z (double)2.0

#define N_X 400
#define N_Y 400
#define N_Z 400

#define H_X (D_X / (N_X - 1))
#define H_Y (D_Y / (N_Y - 1))
#define H_Z (D_Z / (N_Z - 1))

#define H_X_2 (H_X * H_X)
#define H_Y_2 (H_Y * H_Y)
#define H_Z_2 (H_Z * H_Z)

#define A (double)1.0E5
#define EPSILON (double)1.0E-8

double Phi(double x, double y, double z) {
    return x * x + y * y + z * z;
}

double Rho(double x, double y, double z) {
    return 6 - A * Phi(x, y, z);
}

int GetIndex(int x, int y, int z) {
    return x * N_Y * N_Z + y * N_Z + z;
}

double GetX(int i) {
    return X_0 + i * H_X;
}

double GetY(int j) {
    return Y_0 + j * H_Y;
}

double GetZ(int k) {
    return Z_0 + k * H_Z;
}

void DivideAreaIntoLayers(int *layer_heights, int *offsets, int size) {
    int offset = 0;
    for (int i = 0; i < size; ++i) {
        layer_heights[i] = N_X / size;
        if (i < N_X % size) {
            layer_heights[i]++;
        }
        offsets[i] = offset;
        offset += layer_heights[i];
    }
}

void InitializeLayers(double *previous_func, double *current_func, int layer_height, int offset) {
    for (int i = 0; i < layer_height; ++i) {
        for (int j = 0; j < N_Y; j++) {
            for (int k = 0; k < N_Z; k++) {
                bool isBorder = (offset + i == 0) || (j == 0) || (k == 0) ||
                (offset + i == N_X - 1) || (j == N_Y - 1) || (k == N_Z - 1);
                if (isBorder) {
                    previous_func[GetIndex(i, j, k)] = Phi(GetX(offset + i), GetY(j), GetZ(k));
                    current_func[GetIndex(i, j, k)] = Phi(GetX(offset + i), GetY(j), GetZ(k));
                } else {
                    previous_func[GetIndex(i, j, k)] = 0;
                    current_func[GetIndex(i, j, k)] = 0;
                }
            }
        }
    }
}

void SwapFunc(double **previous_func, double **current_func) {
    double *tmp = *previous_func;
    *previous_func = *current_func;
    *current_func = tmp;
}

double CalculateCenter(const double *previous_func, double *current_func, int layer_height, int offset) {
    double f_i = 0.0;
    double f_j = 0.0;
    double f_k = 0.0;
    double tmp_max_diff = 0.0;
    double max_diff = 0.0;
    for (int i = 1; i < layer_height - 1; ++i) {
        for (int j = 1; j < N_Y - 1; ++j) {
            for (int k = 1; k < N_Z - 1; ++k) {
                f_i = (previous_func[GetIndex(i + 1, j, k)] + previous_func[GetIndex(i - 1, j, k)]) / H_X_2;
                f_j = (previous_func[GetIndex(i, j + 1, k)] + previous_func[GetIndex(i, j - 1, k)]) / H_Y_2;
                f_k = (previous_func[GetIndex(i, j, k + 1)] + previous_func[GetIndex(i, j, k - 1)]) / H_Z_2;
                current_func[GetIndex(i, j, k)] = (f_i + f_j + f_k - Rho(GetX(offset + i), GetY(j), GetZ(k))) / (2 / H_X_2 + 2 / H_Y_2 + 2 / H_Z_2 + A);
                tmp_max_diff = fabs(current_func[GetIndex(i, j, k)] - previous_func[GetIndex(i, j, k)]);
                if (tmp_max_diff > max_diff) {
                    max_diff = tmp_max_diff;
                }
            }
        }
    }
    return max_diff;
}

double CalculateBorder(const double *previous_func, double *current_func, double *up_border_layer, double *down_border_layer, int layer_height, int offset, int rank, int size) {
    double f_i = 0.0;
    double f_j = 0.0;
    double f_k = 0.0;
    double tmp_max_diff = 0.0;
    double max_diff = 0.0;
    for (int j = 1; j < N_Y - 1; ++j) {
        for (int k = 1; k < N_Z - 1; ++k) {
            if (rank != 0) {
                f_i = (previous_func[GetIndex(1, j, k)] + up_border_layer[GetIndex(0, j, k)]) / H_X_2;
                f_j = (previous_func[GetIndex(0, j + 1, k)] + previous_func[GetIndex(0, j - 1, k)]) / H_Y_2;
                f_k = (previous_func[GetIndex(0, j, k + 1)] + previous_func[GetIndex(0, j, k - 1)]) / H_Z_2;
                current_func[GetIndex(0, j, k)] = (f_i + f_j + f_k - Rho(GetX(offset), GetY(j), GetZ(k))) / (2 / H_X_2 + 2 / H_Y_2 + 2 / H_Z_2 + A);
                tmp_max_diff = fabs(current_func[GetIndex(0, j, k)] - previous_func[GetIndex(0, j, k)]);
                if (tmp_max_diff > max_diff) {
                    max_diff = tmp_max_diff;
                }
            }
            if (rank != size - 1) {
                f_i = (previous_func[GetIndex(layer_height - 2, j, k)] + down_border_layer[GetIndex(0, j, k)]) / H_X_2;
                f_j = (previous_func[GetIndex(layer_height - 1, j + 1, k)] + previous_func[GetIndex(layer_height - 1, j - 1, k)]) / H_Y_2;
                f_k = (previous_func[GetIndex(layer_height - 1, j, k + 1)] + previous_func[GetIndex(layer_height - 1, j, k - 1)]) / H_Z_2;
                current_func[GetIndex(layer_height - 1, j, k)] = (f_i + f_j + f_k - Rho(GetX(offset + layer_height - 1), GetY(j), GetZ(k))) / (2 / H_X_2 + 2 / H_Y_2 + 2 / H_Z_2 + A);
                tmp_max_diff = fabs(current_func[GetIndex(layer_height - 1, j, k)] - previous_func[GetIndex(layer_height - 1, j, k)]);
                if (tmp_max_diff > max_diff) {
                    max_diff = tmp_max_diff;
                }
            }
        }
    }
    return max_diff;
}

double CalculateMaxDifference(const double *current_func, int layer_height, int offset) {
    double tmp_max_delta = 0.0;
    double max_proc_delta = 0.0;
    double max_delta = 0.0;
    for (int i = 0; i < layer_height; ++i) {
        for (int j = 0; j < N_Y; ++j) {
            for (int k = 0; k < N_Z; ++k) {
                tmp_max_delta = fabs(current_func[GetIndex(i, j, k)] - Phi(GetX(offset + i), GetY(j), GetZ(k)));
                if (tmp_max_delta > max_proc_delta) {
                    max_proc_delta = tmp_max_delta;
                }
            }
        }
    }
    MPI_Allreduce(&max_proc_delta, &max_delta, 1, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD);
    return max_delta;
}

int main(int argc, char **argv) {
    int rank = 0;
    int size = 0;
    double start_time = 0.0;
    double end_time = 0.0;
    double prev_proc_max_diff = EPSILON;
    double max_diff = 0.0;
    int *layer_heights = NULL;
    int *offsets = NULL;
    double *up_border_layer = NULL;
    double *down_border_layer = NULL;
    double *previous_func = NULL;
    double *current_func = NULL;
    MPI_Request send_up_req;
    MPI_Request send_down_req;
    MPI_Request recv_up_req;
    MPI_Request recv_down_req;
    MPI_Request reduce_max_diff_req;
    
    if (N_X < 3 || N_Y < 3 || N_Z < 3) {
        fprintf(stderr, "Incorrect grid size\n");
        return EXIT_FAILURE;
    }
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    layer_heights = malloc(sizeof(int) * size);
    offsets = malloc(sizeof(int) * size);
    DivideAreaIntoLayers(layer_heights, offsets, size);

    previous_func = malloc(sizeof(double) * layer_heights[rank] * N_Y * N_Z);
    current_func = malloc(sizeof(double) * layer_heights[rank] * N_Y * N_Z);
    InitializeLayers(previous_func, current_func, layer_heights[rank], offsets[rank]);

    up_border_layer = malloc(sizeof(double) * N_Y * N_Z);
    down_border_layer = malloc(sizeof(double) * N_Y * N_Z);

    start_time = MPI_Wtime();

    do {
        double tmp_max_diff_1 = 0.0;
        double tmp_max_diff_2 = 0.0;
        MPI_Iallreduce(&prev_proc_max_diff, &max_diff, 1, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD, &reduce_max_diff_req);
        SwapFunc(&previous_func, &current_func);
        if (rank != 0) {
            double *prev_up_border = previous_func;
            MPI_Isend(prev_up_border, N_Y * N_Z, MPI_DOUBLE, rank - 1, rank, MPI_COMM_WORLD, &send_up_req);
            MPI_Irecv(up_border_layer, N_Y * N_Z, MPI_DOUBLE, rank - 1, rank - 1, MPI_COMM_WORLD, &recv_up_req);
        }
        if (rank != size - 1) {
            double *prev_down_border = previous_func + (layer_heights[rank] - 1) * N_Y * N_Z;
            MPI_Isend(prev_down_border, N_Y * N_Z, MPI_DOUBLE, rank + 1, rank, MPI_COMM_WORLD, &send_down_req);
            MPI_Irecv(down_border_layer, N_Y * N_Z, MPI_DOUBLE, rank + 1, rank + 1, MPI_COMM_WORLD, &recv_down_req);
        }
        tmp_max_diff_1 = CalculateCenter(previous_func, current_func, layer_heights[rank], offsets[rank]);
        if (rank != 0) {
            MPI_Wait(&send_up_req, MPI_STATUS_IGNORE);
            MPI_Wait(&recv_up_req, MPI_STATUS_IGNORE);
        }
        if (rank != size - 1) {
            MPI_Wait(&send_down_req, MPI_STATUS_IGNORE);
            MPI_Wait(&recv_down_req, MPI_STATUS_IGNORE);
        }
        tmp_max_diff_2 = CalculateBorder(previous_func, current_func, up_border_layer, down_border_layer, layer_heights[rank], offsets[rank], rank, size);
        MPI_Wait(&reduce_max_diff_req, MPI_STATUS_IGNORE);
        prev_proc_max_diff = fmax(tmp_max_diff_1, tmp_max_diff_2);
    } while (max_diff >= EPSILON);

    SwapFunc(&previous_func, &current_func);
    max_diff = CalculateMaxDifference(current_func, layer_heights[rank], offsets[rank]);

    end_time = MPI_Wtime();

    if (rank == 0) {
        printf("Time: %lf\n", end_time - start_time);
        printf("Max difference: %lf\n", max_diff);
    }

    free(offsets);
    free(layer_heights);
    free(previous_func);
    free(current_func);
    free(up_border_layer);
    free(down_border_layer);

    MPI_Finalize();

    return EXIT_SUCCESS;
}
