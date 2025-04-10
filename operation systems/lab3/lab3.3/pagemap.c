#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <inttypes.h>

void PrintPagemap(const char *pagemap_path) {
    int fd = open(pagemap_path, O_RDONLY);
    if (fd == -1) {
        perror("open");
        return;
    }
    uint64_t entry;
    ssize_t bytes_read;
    while ((bytes_read = read(fd, &entry, sizeof(entry))) > 0) {
        printf("%016" PRIx64 "\n", entry);
    }
    if (bytes_read == -1) {
        perror("read");
    }
    close(fd);
}
    
int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <pid>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    char pagemap_path[256];
    snprintf(pagemap_path, sizeof(pagemap_path), "/proc/%s/pagemap", argv[1]);
    PrintPagemap(pagemap_path);
    return 0;
}
