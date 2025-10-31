#include "../include/socket_utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/socket.h>

volatile sig_atomic_t current_workers = 0;
volatile sig_atomic_t found_flag = 0;

int worker_sockets[MAX_WORKERS];
struct sockaddr_in worker_addrs[MAX_WORKERS];
int worker_pids[MAX_WORKERS];

void sigchld_handler(int signal) {
    pid_t pid;
    int stat;
    while ((pid = waitpid(-1, &stat, WNOHANG)) > 0) {
        current_workers--;
        fprintf(stderr, "[X] Worker [pid=%d] desconectado. Activos: %d\n", pid, current_workers);
    }
}

int createSocket(int port, int type) {
    int socketfd;
    struct sockaddr_in address;
    socklen_t length;

    if ((socketfd = socket(PF_INET, type, 0)) == -1) {
        perror("Error: socket");
        exit(2);
    }

    memset(&address, 0, sizeof(address));
    address.sin_port = htons(port);
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_family = PF_INET;

    int opt = 1;
    setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    if (bind(socketfd, (struct sockaddr *)&address, sizeof(address)) == -1) {
        perror("Error: bind");
        exit(3);
    }

    length = sizeof(address);
    getsockname(socketfd, (struct sockaddr *)&address, &length);
    printf("Socket creado en puerto %d\n", ntohs(address.sin_port));
    return socketfd;
}

void stop_all_workers() {
    for (int i = 0; i < current_workers; i++) {
        if (worker_sockets[i] > 0) {
            const char *msg = "STOP\n";
            write(worker_sockets[i], msg, strlen(msg));
             printf("Enviando orden de detención [STOP] a worker: %s [pid=%d]\n",
                   inet_ntoa(worker_addrs[i].sin_addr),
                   worker_pids[i]);
        }
    }
}
