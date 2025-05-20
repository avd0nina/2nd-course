#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 12345
#define BUFFER_SIZE 10

int main() {
    int socket_fd;
    struct sockaddr_in server_addr, client_addr;
    char buffer[BUFFER_SIZE];
    socklen_t addr_len = sizeof(client_addr);
    if ((socket_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) { // создаем udp-сокет
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    memset(&server_addr, 0, sizeof(server_addr)); // настраиваем структуру
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY; // любой IP-адрес
    server_addr.sin_port = htons(PORT); // преобразует номер порта из порядка байтов хоста в сетевой порядок байтов (big-endian)
    if (bind(socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) { // привязываем сокет к серверу
        perror("bind failed");
        close(socket_fd);
        exit(EXIT_FAILURE);
    }
    printf("UDP echo-server is running on port %d...\n", PORT);
    while (1) {
        int n = recvfrom(socket_fd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&client_addr, &addr_len); // получаем данные от клиента
        if (n < 0) {
            perror("recvfrom failed");
            continue;
        }
        buffer[n] = '\0';
        printf("Got message: %s\n", buffer);
        sendto(socket_fd, buffer, n, 0, (struct sockaddr *)&client_addr, addr_len); // отправляем данные обратно клиенту
    }
    close(socket_fd);
    return 0;
}
