#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

#define PAGE_SIZE 4096

void AllocateOnHeap() {
    while (1) {
        char *buffer = malloc(PAGE_SIZE);
        if (buffer == NULL) {
            perror("malloc");
            exit(EXIT_FAILURE);
        }
        printf("Allocation: %p\n", &buffer[0]);
        sleep(1);
    }
}


int main(int argc, char **argv) {
    printf("PID: %d\n", getpid());
    sleep(10);
    AllocateOnHeap();
    return EXIT_SUCCESS;
}

