#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include <signal.h>

#define SUCCESS 0
#define ERROR (-1)
#define PAGE_SIZE 4096
#define NO_FD (-1)

void SegfaultHandler(int signum) {
    printf("Caught signal %d: Segmentation fault\n", signum);
    exit(EXIT_FAILURE);
}

void AllocateAddressRegion() {
    int ret = SUCCESS;
    char *address_region;
    address_region = mmap(NULL, 10 * PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, NO_FD, 0);
    if (address_region == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }
    ret = mprotect(address_region, 10 * PAGE_SIZE, PROT_READ);
    if (ret == ERROR) {
        perror("mprotect");
        exit(EXIT_FAILURE);
    }
    printf("Changed memory protection: PROT_READ\n");
    *address_region = 'a';
    sleep(10);
}

int main(int argc, char **argv) {
    signal(SIGBUS, SegfaultHandler);
    printf("PID: %d\n", getpid());
    sleep(10);
    AllocateAddressRegion();
    return EXIT_SUCCESS;
}

