#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_PORT 12345
#define BUFFER_SIZE 1024

int main() {
    int socket_fd;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    if ((socket_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) { // создаем udp-сокет
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    memset(&server_addr, 0, sizeof(server_addr)); // настраиваем стрктуру
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // localhost, этот компьютер
    while (1) {
        printf("Enter a message (or 'exit'): ");
        fgets(buffer, BUFFER_SIZE, stdin);
        if (strncmp(buffer, "exit", 4) == 0) {
            break;
        }
        sendto(socket_fd, buffer, strlen(buffer), 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
        int n = recvfrom(socket_fd, buffer, BUFFER_SIZE, 0, NULL, NULL);
        buffer[n] = '\0';
        printf("Server response: %s\n", buffer);
    }
    close(socket_fd);
    return 0;
}
