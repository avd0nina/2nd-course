#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>

#define PORT 12345
#define BUFFER_SIZE 1024
#define BACKLOG 10
#define MAX_FDS 2048

int main() {
    int listen_socket_fd, client_socket_fd;
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);
    char buffer[BUFFER_SIZE];
    int nbytes;
    struct pollfd fds[MAX_FDS];
    int nfds = 1; // количество сокетов в массиве fds, которые нужно проверить
    int i;
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
    memset(fds, 0, sizeof(fds)); // очищаем массив
    fds[0].fd = listen_socket_fd; // устанавливаем первый элемент массива fds для слежения за слушающим сокетом
    fds[0].events = POLLIN; // сообщи, если есть данные для чтения или новое подключение
    printf("TCP echo server started on port %d\n", PORT);
    while (1) {
        int activity = poll(fds, nfds, -1); // проверяем, какие сокеты в массиве fds имеют события
        if (activity < 0) {
            perror("poll");
            exit(EXIT_FAILURE);
        }
        for (i = 0; i < nfds; i++) {
            if (fds[i].revents == 0) { // пропускаем, если нет событий
                continue;
            }
            if (fds[i].fd == listen_socket_fd) {
                if ((client_socket_fd = accept(listen_socket_fd, (struct sockaddr*)&addr, &addr_len)) < 0) { // принимаем клиента
                    perror("accept");
                    continue;
                }
                if (nfds >= MAX_FDS) {
                    fprintf(stderr, "Too many clients\n");
                    close(client_socket_fd);
                    continue;
                }
                fds[nfds].fd = client_socket_fd; // добавляем клиентский сокет
                fds[nfds].events = POLLIN;
                nfds++;
                printf("New connection from %s:%d on socket %d\n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port), client_socket_fd);
            } else {
                nbytes = read(fds[i].fd, buffer, sizeof(buffer)); // читаем данные от клиента
                if (nbytes <= 0) {
                    if (nbytes == 0) {
                        printf("Socket %d hung up\n", fds[i].fd);
                    } else {
                        perror("recv");
                    }
                    close(fds[i].fd);
                    fds[i].fd = -1;
                } else {
                    write(fds[i].fd, buffer, nbytes); // отправляем прочитанные данные обратно клиенту
                }
            }
        }
        int j = 1; // начинаем с 1, чтобы не трогать слушающий сокет в fds[0]. мы хотим убрать пустые места в массиве, где сокеты закрыты
        for (i = 1; i < nfds; i++) {
            if (fds[i].fd != -1) { // проверяет активен ли сокет
                if (i != j) {
                    fds[j] = fds[i];
                }
                j++;
            }
        }
        nfds = j;
    }
    close(listen_socket_fd);
    return 0;
}
