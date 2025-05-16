#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

int global_var = 42;

int main() {
    int local_var = 10;
    printf("Родительский PID: %d\n", getpid());
    printf("Глобальная переменная: адрес=%p, значение=%d\n", &global_var, global_var);
    printf("Локальная переменная: адрес=%p, значение=%d\n", &local_var, local_var);
    pid_t pid = fork();
    if (pid == -1) {
        perror("Ошибка fork()");
        return 1;
    }
    if (pid == 0) {
        printf("Дочерний PID: %d\n", getpid());
        printf("Дочерний PPID (родительский PID): %d\n", getppid());
        printf("Дочерний: Глобальная переменная: адрес=%p, значение=%d\n", &global_var, global_var);
        printf("Дочерний: Локальная переменная: адрес=%p, значение=%d\n", &local_var, local_var);

        global_var = 100;
        local_var = 50;
        printf("\nДочерний: Изменённые значения: global=%d, local=%d\n", global_var, local_var);
        printf("Дочерний: Глобальная переменная: адрес=%p, значение=%d\n", &global_var, global_var);
        printf("Дочерний: Локальная переменная: адрес=%p, значение=%d\n", &local_var, local_var);
        sleep(60);
        exit(5);
    }
    else {
        printf("\nРодительский: Глобальная переменная: значение=%d\n", global_var);
        printf("Родительский: Локальная переменная: значение=%d\n", local_var);
        sleep(30);
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
            printf("Дочерний процесс завершился с кодом: %d\n", WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
            printf("Дочерний процесс убит сигналом: %d\n", WTERMSIG(status));
        } else {
            printf("Дочерний процесс завершился по неизвестной причине\n");
        }
    }
    return 0;
}
