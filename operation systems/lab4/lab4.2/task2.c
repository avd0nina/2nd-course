#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define ERROR (-1)

int main(int argc, char **argv) {
    int ret;
    printf("PID: %d\n", getpid());
    sleep(30);
    ret = execv(argv[0], argv);
    if (ret == ERROR) {
        perror("execv");
        return EXIT_FAILURE;
    }
    printf("Hello world\n");
    return EXIT_SUCCESS;
}
