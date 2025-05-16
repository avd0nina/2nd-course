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
    char *address_region;
    address_region = mmap(NULL, 10 * PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, NO_FD, 0);
    if (address_region == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }
    printf("Created address region: %p\n", &address_region[0]);
    sleep(10);
}

int main(int argc, char **argv) {
    printf("PID: %d\n", getpid());
    sleep(10);
    AllocateAddressRegion();
    return EXIT_SUCCESS;
}

