#define _GNU_SOURCE
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <string.h>
#include <sys/wait.h>

#define STACK_SIZE (1024 * 1024)
#define RECURSION_DEPTH 10
#define FILE_PATH "shared_stack.bin"

char *mapped_stack;

void RecursiveFunction(int depth) {
    if (depth == 0) {
        return;
    }
    char message[] = "hello world";
    write(STDOUT_FILENO, message, sizeof(message) - 1);
    write(STDOUT_FILENO, "\n", 1);
    RecursiveFunction(depth - 1);
}

int ChildFunc(void *arg) {
    RecursiveFunction(RECURSION_DEPTH);
    return 0;
}

int main() {
    int fd = open(FILE_PATH, O_RDWR | O_CREAT | O_TRUNC, 0600);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }
    if (ftruncate(fd, STACK_SIZE) == -1) {
        perror("ftruncate");
        close(fd);
        exit(EXIT_FAILURE);
    }
    mapped_stack = mmap(NULL, STACK_SIZE, PROT_READ | PROT_WRITE,
                        MAP_SHARED, fd, 0);
    if (mapped_stack == MAP_FAILED) {
        perror("mmap");
        close(fd);
        exit(EXIT_FAILURE);
    }
    void *stack_top = mapped_stack + STACK_SIZE;
    pid_t pid = clone(ChildFunc, stack_top, SIGCHLD, NULL);
    if (pid == -1) {
        perror("clone");
        munmap(mapped_stack, STACK_SIZE);
        close(fd);
        exit(EXIT_FAILURE);
    }
    waitpid(pid, NULL, 0);
    munmap(mapped_stack, STACK_SIZE);
    close(fd);
    printf("Parent: clone child completed\n");
    return 0;
}
