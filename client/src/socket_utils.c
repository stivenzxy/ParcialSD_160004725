#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "../include/socket_utils.h"

int connect_to_server(const char *ip, int port)
{
    int sock;
    struct sockaddr_in server_addr;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Error al crear socket");
        return -1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, ip, &server_addr.sin_addr) <= 0)
    {
        perror("Dirección IP inválida");
        close(sock);
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("Conexión fallida");
        close(sock);
        return -1;
    }

    printf("[OK] Conectado al líder %s:%d\n", ip, port);

    pid_t pid = getpid();
    char pid_msg[64];
    sprintf(pid_msg, "PID:%d\n", pid);
    write(sock, pid_msg, strlen(pid_msg));

    return sock;
}
