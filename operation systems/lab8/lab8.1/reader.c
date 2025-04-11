#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main() {
    FILE *file = fopen("secret.txt", "r");
    if (!file) {
        perror("Ошибка при открытии файла");
        return 1;
    }

    char buffer[256];
    fgets(buffer, sizeof(buffer), file);
    fclose(file);

    printf("Содержимое файла: %s\n", buffer);
    printf("Real UID: %d\n", getuid());
    printf("Effective UID: %d\n", geteuid());

    return 0;
}
