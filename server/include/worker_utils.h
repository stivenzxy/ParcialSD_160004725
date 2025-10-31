#ifndef WORKER_UTILS_H
#define WORKER_UTILS_H

#include <netinet/in.h>

#define MAXLINE 4096

void receive_worker(int index,int sock, struct sockaddr_in client_addr);

#endif
