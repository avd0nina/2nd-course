#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main() {
    pid_t pid = fork();
    if (pid == 0) {
        printf("Дочерний PID: %d\n", getpid());
        exit(0);
    } else {
        printf("Родительский PID: %d\n", getpid());
        sleep(30);
        printf("Родитель завершен\n");
    }
    return 0;
}
