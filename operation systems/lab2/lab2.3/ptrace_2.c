#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/user.h>
#include <sys/reg.h>
#include <sys/syscall.h>
#include <errno.h>

void trace_syscalls(pid_t child_pid) {
    int status;
    int in_syscall = 0;
    struct user_regs_struct regs;

    waitpid(child_pid, &status, 0);
    ptrace(PTRACE_SETOPTIONS, child_pid, 0, PTRACE_O_TRACESYSGOOD);

    while (1) {
        ptrace(PTRACE_SYSCALL, child_pid, 0, 0);
        waitpid(child_pid, &status, 0);
        if (WIFEXITED(status)) {
            break;
        }
        if (WIFSTOPPED(status) && (WSTOPSIG(status) & 0x80)) {
            ptrace(PTRACE_GETREGS, child_pid, NULL, &regs);
            if (in_syscall == 0) {
                printf("\n[Syscall Enter] %lld(", regs.orig_rax);
                printf("rdi: 0x%llx, rsi: 0x%llx, rdx: 0x%llx, r10: 0x%llx, r8: 0x%llx, r9: 0x%llx)\n",
                       regs.rdi, regs.rsi, regs.rdx, regs.r10, regs.r8, regs.r9);
                in_syscall = 1;
            } else {
                printf("[Syscall Exit] return: 0x%llx (%lld)\n", regs.rax, regs.rax);
                in_syscall = 0;
            }
        }
    }
}

int main(void) {
    pid_t child_pid = fork();

    if (child_pid == 0) {
        ptrace(PTRACE_TRACEME, 0, NULL, NULL);
        raise(SIGSTOP);
        execl("/bin/ls", "ls", NULL);
        perror("execl");
        exit(1);
    } else {
        trace_syscalls(child_pid);
    }
    return 0;
}

