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
    int in_syscall = 0; // на входе 0, на выходе 1
    while (1) {
        wait(&status);
        if (WIFEXITED(status)) {
            break;
        }
        ptrace(PTRACE_GETREGS, child_pid, NULL, &regs);
        if (in_syscall == 0) { // вход
            printf("\n[Syscall Enter] %lld(", regs.orig_rax);
            printf("rdi: 0x%llx, rsi: 0x%llx, rdx: 0x%llx, r10: 0x%llx, r8: 0x%llx, r9: 0x%llx)\n",
                   regs.rdi, regs.rsi, regs.rdx, regs.r10, regs.r8, regs.r9);
            in_syscall = 1;
        } else { // выход
            printf("[Syscall Exit] return: 0x%llx\n", regs.rax);
            in_syscall = 0;
        }
        ptrace(PTRACE_SYSCALL, child_pid, NULL, NULL);
    }
}

int main(void) {
    pid_t child_pid = fork();
    if (child_pid == 0) {
        ptrace(PTRACE_TRACEME, 0, NULL, NULL);
        execl("/bin/ls", "ls", NULL);
    } else {
        trace_syscalls(child_pid);
    }
    return 0;
}
