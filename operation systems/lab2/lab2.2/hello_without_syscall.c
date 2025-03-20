.data

str:
    .ascii "Hello world\n"
    len = . - str

.text
    .global _start

_start:
    movq $1, %rax // 1 - номер системного вызова write
    movq $1, %rdi // 1 - файловый дескриптор stdout
    movq $str, %rsi
    movq $len, %rdx
    syscall // write

    movq $60, %rax // 60 - номер системного вызова sys_exit
    xorq %rdi, %rdi // Обнуляем rdi (код возврата 0)
    syscall // exit
