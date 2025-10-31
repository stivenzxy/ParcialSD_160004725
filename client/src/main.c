#include <stdio.h>
#include <stdlib.h>
#include "../include/worker.h"

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        fprintf(stderr, "Uso: %s <IP_LIDER> <PUERTO>\n", argv[0]);
        exit(1);
    }

    const char *ip = argv[1];
    int port = atoi(argv[2]);

    start_worker(ip, port);
    return 0;
}
