#include "../include/worker_utils.h"
#include "../include/socket_utils.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>

#define COLOR_GREEN "\033[1;32m"
#define COLOR_RED "\033[1;31m"
#define COLOR_YELLOW "\033[1;33m"
#define COLOR_RESET "\033[0m"

void receive_worker(int index, int sock, struct sockaddr_in client_addr)
{
    char buffer[MAXLINE];
    ssize_t n;
    char *worker_ip = inet_ntoa(client_addr.sin_addr);
    int pid = worker_pids[index];

    while ((n = read(sock, buffer, sizeof(buffer) - 1)) > 0)
    {
        buffer[n] = '\0';
        printf("[Worker #%d | PID=%d | %s]: %s\n", index + 1, pid, worker_ip, buffer);

        const char *message = "FOUND";
        if (strncmp(buffer, message, strlen(message)) == 0)
        {
            if (strncmp(buffer, message, strlen(message)) == 0)
            {
                if (!found_flag)
                {
                    found_flag = 1;
                    printf(COLOR_GREEN "\nHash encontrado por worker #%d (PID=%d, IP=%s)\n" COLOR_RESET,
                           index + 1, pid, worker_ip);
                    printf(COLOR_YELLOW ">>> %s\n" COLOR_RESET, buffer);
                    stop_all_workers();
                    exit(0);
                }
            }
        }
    }

    close(sock);
    printf("[X] Worker #%d desconectado (PID=%d, IP=%s)\n", index + 1, pid, worker_ip);
}