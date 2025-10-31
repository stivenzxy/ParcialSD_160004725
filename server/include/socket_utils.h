#ifndef SOCKET_UTILS_H
#define SOCKET_UTILS_H

#include <netinet/in.h>
#include <signal.h>

extern int worker_sockets[];
extern int worker_pids[]; 
extern struct sockaddr_in worker_addrs[];
extern volatile sig_atomic_t current_workers;
extern volatile sig_atomic_t found_flag;

#define MAX_WORKERS 4

int createSocket(int port, int type);
void sigchld_handler(int signal);
void stop_all_workers(); 

#endif
