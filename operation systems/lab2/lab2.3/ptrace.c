#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/user.h>
#include <sys/reg.h>
#include <sys/syscall.h>

void trace_syscalls(pid_t child_pid) {
    int status;
    struct user_regs_struct regs;
    while (1) {
        wait(&status); // Ожидаем остановки дочернего процесса
        if (WIFEXITED(status)) {
            break;
        }
        // Получаем регистры дочернего процесса
        ptrace(PTRACE_GETREGS, child_pid, NULL, &regs);
        // Выводим номер системного вызова
        printf("Syscall %ld called\n", regs.orig_rax);
        // Продолжаем выполнение дочернего процесса до следующего системного вызова
        ptrace(PTRACE_SYSCALL, child_pid, NULL, NULL);
    }
}

int main(void) {
    pid_t child_pid;
    child_pid = fork(); // родительсий процесс получает pid дочернего процесса, дочерний процесс получает 0
    if (child_pid == 0) {
        // Дочерний процесс
        ptrace(PTRACE_TRACEME, 0, NULL, NULL);
        execl("/bin/ls", "ls", NULL);
    } else {
        // Родительский процесс
        trace_syscalls(child_pid);
    }
    return 0;
}
