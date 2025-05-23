#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 12345
#define BUFFER_SIZE 1024
#define BACKLOG 10

int main() {
    int listen_socket_fd, client_socket_fd, max_fd, i;
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);
    char buffer[BUFFER_SIZE];
    int nbytes;
    fd_set master_set, read_fds;
    if ((listen_socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    int opt = 1;
    setsockopt(listen_socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(PORT);
    if (bind(listen_socket_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind");
        close(listen_socket_fd);
        exit(EXIT_FAILURE);
    }
    if (listen(listen_socket_fd, BACKLOG) < 0) {
        perror("listen");
        close(listen_socket_fd);
        exit(EXIT_FAILURE);
    }
    FD_ZERO(&master_set); // очищаем набор дескрипторов
    FD_SET(listen_socket_fd, &master_set); // select должен отслеживать события на listen_socket_fd
    max_fd = listen_socket_fd;
    printf("TCP echo server started on port %d\n", PORT);
    while (1) {
        read_fds = master_set; // копия, потому что когда будем передавать в select, он изменит переданный ему набор дескрипторов, оставляя только те, которые "готовы"
        int activity = select(max_fd + 1, &read_fds, NULL, NULL, NULL);
        if (activity < 0) {
            perror("select");
            exit(EXIT_FAILURE);
        }
        if (FD_ISSET(listen_socket_fd, &read_fds)) { // если listen_socket_fd есть в read_fds, то кто-то пытается подключиться к серверу (клиент).
            if ((client_socket_fd = accept(listen_socket_fd, (struct sockaddr*)&addr, &addr_len)) < 0) { // принимает новое подключение от клиента
                perror("accept");
                continue;
            }
            FD_SET(client_socket_fd, &master_set); // добавляем клиентский сокет
            if (client_socket_fd > max_fd) {
                max_fd = client_socket_fd;
            }
            printf("New connection from %s:%d on socket %d\n",
                   inet_ntoa(addr.sin_addr), ntohs(addr.sin_port), client_socket_fd);
        }
        for (i = 0; i <= max_fd; i++) { // проверяем, есть ли данные на клиентских сокетах
            if (i == listen_socket_fd) {
                continue;
            }
            if (FD_ISSET(i, &read_fds)) { // если дескриптор есть в read_fds, значит на этом сокете есть данные для чтения
                nbytes = read(i, buffer, sizeof(buffer));
                if (nbytes <= 0) {
                    if (nbytes == 0) {
                        printf("Socket %d hung up\n", i); // клиент с сокетом i отключился
                    } else {
                        perror("recv");
                    }
                    close(i);
                    FD_CLR(i, &master_set); // убирает сокет i из набора master_set
                } else {
                    write(i, buffer, nbytes);
                }
            }
        }
    }
    close(listen_socket_fd);
    return 0;
}
