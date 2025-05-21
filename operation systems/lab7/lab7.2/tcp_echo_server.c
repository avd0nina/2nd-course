#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>

#define PORT 12345
#define BUFFER_SIZE 10
#define BACKLOG 10 // максимальная длина очереди входящих соединений

void HandleClient(int client_socket_fd) {
    char buffer[BUFFER_SIZE];
    int n;
    while ((n = read(client_socket_fd, buffer, BUFFER_SIZE)) > 0) {
        buffer[n] = '\0';
        printf("Got message: %s\n", buffer);
        write(client_socket_fd, buffer, n);  // эхо-ответ
    }
    close(client_socket_fd);
    exit(0); // завершаем дочерний процесс
}

int main() {
    int server_socket_fd, client_socket_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    server_socket_fd = socket(AF_INET, SOCK_STREAM, 0); // создаем tcp-сокет
    if (server_socket_fd < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    memset(&server_addr, 0, sizeof(server_addr)); // настраиваем структуру
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);
    if (bind(server_socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) { // привязываем сокет к серверу
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_socket_fd, BACKLOG) < 0) { // переводим сокет в режим прослушивания
        perror("listen failed");
        exit(EXIT_FAILURE);
    }
    printf("TCP echo-server is running on port %d...\n", PORT);
    while (1) {
        client_socket_fd = accept(server_socket_fd, (struct sockaddr *)&client_addr, &client_len); // принимает TCP-соединение и возвращает новый сокет для общения с клиентом.
        if (client_socket_fd < 0) {
            perror("accept failed");
            continue;
        }
        if (fork() == 0) { // создаем дочерний процесс, который обрабатывает соединения, а роидтель продолжает слушать
            close(server_socket_fd); // дочернему процессу серверный сокет не нужен
            HandleClient(client_socket_fd);
        } else {
            close(client_socket_fd); // родителю не нужен клиентский сокет
        }
    }
    close(server_socket_fd);
    return 0;
}
