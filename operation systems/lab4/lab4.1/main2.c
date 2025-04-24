#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>

int global_not_inited_var;
int global_inited_var = 1;
const int global_const_var = 2;

int* InitLocalVar() {
    int local_inited_var = 4;
    printf("local_inited_var:        %d\n", local_inited_var);
    return &local_inited_var;
}

int main() {
    int local_var;
    static int static_var;
    const int local_const_var = 3;
//    int *local_inited_var_address = InitLocalVar();
    printf("pid: %d\n\n", getpid());
    printf("global_not_inited_var:  %p, %d\n", &global_not_inited_var, global_not_inited_var);
    printf("global_inited_var:      %p, %d\n", &global_inited_var, global_inited_var);
    printf("global_const_var:       %p, %d\n", &global_const_var, global_const_var);
    printf("local_var:              %p, %d\n", &local_var, local_var);
    printf("static_var:             %p, %d\n", &static_var, static_var);
    printf("local_const_var:        %p, %d\n", &local_const_var, local_const_var);
    InitLocalVar();
    int *local_inited_var_address = InitLocalVar();
    printf("local_inited_var: %p, %d\n", local_inited_var_address, *local_inited_var_address);
    sleep(60);
    return EXIT_SUCCESS;
}
