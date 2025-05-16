#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

#define PAGE_SIZE 4096

void AllocateOnStack() {
    char buffer[PAGE_SIZE];
    printf("Allocation: %p\n", &buffer[0]);
    sleep(1);
    AllocateOnStack();
}

int main(int argc, char **argv) {
    printf("PID: %d\n", getpid());
    sleep(10);
    AllocateOnStack();
    return EXIT_SUCCESS;
}

