#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <mpi.h>
#include <pthread.h>

#define L 2048
#define NUM_OF_TASKS 2048
#define NUM_OF_ITERATIONS 16
#define ASK_FOR_TASKS_TAG 128
#define SEND_NUM_OF_TASKS_TAG 256
#define SEND_TASKS_TAG 512
#define IS_DONE 8

int size;
int rank;
double global_result = 0;
bool worker_is_done = false;
int* task_list;
int completed_tasks;
int remained_tasks;
int supplied_tasks;
pthread_mutex_t lock;

void InitializeTasks(int* tasks, int iteration) {
    for (int i = 0; i < NUM_OF_TASKS; i++) {
         tasks[i] = abs(50 - i % 100) * abs(rank) * L;
    }
}

void CalculateTasks(const int* tasks) {
    int weight = 0;
    for (int i = 0; i < remained_tasks; i++) {
        pthread_mutex_lock(&lock);
        weight = tasks[i];
        pthread_mutex_unlock(&lock);
        for (int j = 0; j < weight; j++) {
            global_result += sin(j);
        }
        pthread_mutex_lock(&lock);
        completed_tasks++;
        pthread_mutex_unlock (&lock);
    }
    remained_tasks = 0;
}

void* Work(void* ignored) {
    double start_iteration, end_iteration;
    double iteration_time, min_iteration_time, max_iteration_time;
    double disbalance, disbalance_percent;
    task_list = (int*)calloc(NUM_OF_TASKS, sizeof(int));
    for (int i = 0; i < NUM_OF_ITERATIONS; i++) {
        InitializeTasks(task_list, i);
        start_iteration = MPI_Wtime();
        completed_tasks = 0;
        supplied_tasks = 0;
        remained_tasks = NUM_OF_TASKS;
        CalculateTasks(task_list);
        int responsed = 0;
        bool enough_tasks = false;
        while (!enough_tasks) {
            int previous_process = (rank - 1 + size) % size;
            MPI_Send(&rank, 1, MPI_INT, previous_process, ASK_FOR_TASKS_TAG, MPI_COMM_WORLD);
            MPI_Recv(&responsed, 1, MPI_INT, previous_process, SEND_NUM_OF_TASKS_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            if (responsed == 0) {
                enough_tasks = true;
                break;
            }
            supplied_tasks = responsed;
            MPI_Recv(task_list, supplied_tasks, MPI_INT, previous_process, SEND_TASKS_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            pthread_mutex_lock(&lock);
            remained_tasks = supplied_tasks;
            pthread_mutex_unlock(&lock);
            CalculateTasks(task_list);
        }
        end_iteration = MPI_Wtime();
        iteration_time = end_iteration - start_iteration;
        MPI_Allreduce(&iteration_time, &max_iteration_time, 1, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD);
        MPI_Allreduce(&iteration_time, &min_iteration_time, 1, MPI_DOUBLE, MPI_MIN, MPI_COMM_WORLD);
        MPI_Barrier(MPI_COMM_WORLD);
        disbalance = max_iteration_time - min_iteration_time;
        disbalance_percent = (disbalance / max_iteration_time) * 100;
        printf("\n");
        printf("rank = %d\n iteration = %d\n time = %f\n task_list = %d\n global_result = %.2f\n", rank, i, iteration_time, completed_tasks, global_result);
        if (rank == 0) printf("iteration #%d\n disbalance = %.2f disbalance_percent = %.2f\n", i, disbalance, disbalance_percent);
        printf("\n");
    }
    pthread_mutex_lock(&lock);
    worker_is_done = true;
    pthread_mutex_unlock(&lock);
    int done = IS_DONE;
    MPI_Send(&done, 1, MPI_INT, rank, ASK_FOR_TASKS_TAG, MPI_COMM_WORLD);
    free(task_list);
    pthread_exit(NULL);
}

void* Receive(void* ignored) {
    int done;
    int tasks_to_send;
    MPI_Barrier(MPI_COMM_WORLD);
    while (!worker_is_done) {
        MPI_Recv(&done, 1, MPI_INT, MPI_ANY_SOURCE, ASK_FOR_TASKS_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        if (done == IS_DONE) {
            break;
        }
        pthread_mutex_lock(&lock);
        int tasks_left = remained_tasks - completed_tasks;
        tasks_to_send = remained_tasks / (size * 2);
        if (tasks_left >= 1 && tasks_to_send >= 1) {
            remained_tasks -= tasks_to_send;
            MPI_Send(&tasks_to_send, 1, MPI_INT, done, SEND_NUM_OF_TASKS_TAG, MPI_COMM_WORLD);
            MPI_Send(&task_list[remained_tasks - tasks_to_send], tasks_to_send, MPI_INT, done, SEND_TASKS_TAG, MPI_COMM_WORLD);
        } else {
            tasks_to_send = 0;
            MPI_Send(&tasks_to_send, 1, MPI_INT, done, SEND_NUM_OF_TASKS_TAG, MPI_COMM_WORLD);
        }
        pthread_mutex_unlock(&lock);
    }
    pthread_exit(NULL);
}

int main(int argc, char **argv) {
  int provided;
  MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);
  if (provided != MPI_THREAD_MULTIPLE) {
    printf("Can't set MPI_THREAD_MULTIPLE.\n");
    MPI_Finalize();
    return 1;
  }
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  pthread_mutex_init(&lock, NULL);
  pthread_attr_t attr;
  if (pthread_attr_init(&attr) != 0) {
    printf("Can't initialize attr\n");
    MPI_Finalize();
    return 1;
  }
  if (pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE)) {
    printf("Can't set PTHREAD_CREATE_JOINABLE to attr.\n");
    MPI_Finalize();
    return 1;
  }
  double start_time, end_time;
  start_time = MPI_Wtime();
  pthread_t worker, receiver;
  if (pthread_create(&worker, &attr, Work, NULL) != 0) {
      printf("Can't create worker thread.\n");
      MPI_Finalize();
      return 1;
  }
  if (pthread_create(&receiver, &attr, Receive, NULL) != 0) {
      printf("Can't create receiver thread.\n");
      MPI_Finalize();
      return 1;
  }
  if (pthread_join(worker, NULL) != 0) {
      printf("Can't join worker thread.\n");
      MPI_Finalize();
      return 1;
  }
  if (pthread_join(receiver, NULL) != 0) {
      printf("Can't join receiver thread.\n");
      MPI_Finalize();
      return 1;
  }
  end_time = MPI_Wtime();
  if (rank == 0) { 
      printf("Time spent: %.2lf seconds.\n", end_time - start_time);
  }
  pthread_attr_destroy(&attr);
  pthread_mutex_destroy(&lock);
  MPI_Finalize();
  return 0;
}
