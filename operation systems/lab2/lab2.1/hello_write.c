#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#define ERROR (-1)

int main(void) {
    char buffer[] = "Hello, World!\n";
    ssize_t result;
    ssize_t written_bytes = 0;
    ssize_t length = sizeof(buffer);

    while (written_bytes < length) {
        result = write(STDOUT_FILENO, buffer + written_bytes, length - written_bytes);
        if (result == ERROR) {
            fprintf(stderr, "Error: %s\n", strerror(errno));
            return EXIT_FAILURE;
        }
        written_bytes += result;
    }
    return EXIT_SUCCESS;
}
