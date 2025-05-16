#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

#define SUCCESS 0
#define ERROR (-1)
#define PAGE_SIZE 4096
#define NO_FD (-1)

void AllocateAddressRegion() {
    int ret = SUCCESS;
//    char *address_region;
//    address_region = mmap(NULL, 10 * PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, NO_FD, 0);
//    if (address_region == MAP_FAILED) {
//        perror("mmap");
//        exit(EXIT_FAILURE);
//    }
//    ret = munmap(address_region + 3 * PAGE_SIZE, 3 * PAGE_SIZE);
//    if (ret == ERROR) {
//        perror("munmap");
//        exit(EXIT_FAILURE);
//    }
    char *region[10];
    for (int i = 0; i < 10; ++i) {
        region[i] = mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, NO_FD, 0);
        if (region[i] == MAP_FAILED) {
            perror("mmap");
            exit(EXIT_FAILURE);
        }
    }
    for (int i = 4; i < 7; ++i) {
        if (munmap(region[i], PAGE_SIZE) == -1) {
            perror("munmap");
            exit(EXIT_FAILURE);
        }
    }
    printf("Deallocated address region\n");
    sleep(10);
}

int main(int argc, char **argv) {
    printf("PID: %d\n", getpid());
    sleep(10);
    AllocateAddressRegion();
    return EXIT_SUCCESS;
}

