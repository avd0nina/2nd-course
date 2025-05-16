#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main() {
    pid_t pid = fork();
    if (pid == 0) {
        sleep(20);
        printf("Дочерний PID: %d (PPID теперь: %d)\n", getpid(), getppid());
        exit(0);
    } else {
        printf("Родительский PID: %d\n", getpid());
        exit(0);
    }
}
