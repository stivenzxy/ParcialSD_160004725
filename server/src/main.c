#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <sys/select.h>
#include <arpa/inet.h>

#include "../include/socket_utils.h"
#include "../include/worker_utils.h"
#include "../include/mining_utils.h"

#define MAX_PATRON 32
#define TEXTO_BASE 256
#define PORT 9090
#define TRUE 1

int main(int argc, char *argv[])
{
    int max_workers = MAX_WORKERS;
    int port = PORT;
    char texto_base[TEXTO_BASE];
    int relleno_len;
    int tipo_cond;
    char patron_cond[MAX_PATRON];
    char condicion[32];


    if (argc < 3)
    {
        printf("USO: %s [puerto] [num_workers]\n", argv[0]);
        printf("Ejemplo: %s 9090 3\n", argv[0]);
        exit(1);
    }

    if (argc > 1)
        port = atoi(argv[1]);
    if (argc > 2)
        max_workers = atoi(argv[2]);

    printf("Ingrese el texto base a minar: ");
    fgets(texto_base, sizeof(texto_base), stdin);
    texto_base[strcspn(texto_base, "\n")] = 0;

    do
    {
        printf("Ingrese la longitud del relleno [1-6]: ");
        scanf("%d", &relleno_len);
        if (relleno_len < 1 || relleno_len > 6)
            printf("Valor inválido. Debe estar entre 1 y 6.\n");
    } while (relleno_len < 1 || relleno_len > 6);

    printf("Tipo de condición:\n 1. Empieza con\n 2. Termina con\n 3. Contiene\nSeleccione: ");
    scanf("%d", &tipo_cond);

    printf("Ingrese el patrón de condición (ejemplo '00'): ");
    scanf("%s", patron_cond);

    if (tipo_cond == 1)
        sprintf(condicion, "startswith:%s", patron_cond);
    else if (tipo_cond == 2)
        sprintf(condicion, "endswith:%s", patron_cond);
    else
        sprintf(condicion, "contains:%s", patron_cond);

    printf("\n--- Configuración ---\n");
    printf("Texto base: %s\n", texto_base);
    printf("Relleno: %d caracteres (0-9, a-z)\n", relleno_len);
    printf("Condición: %s\n", condicion);
    printf("----------------------\n\n");

    int sock_escucha = createSocket(port, SOCK_STREAM);
    signal(SIGCHLD, sigchld_handler);
    listen(sock_escucha, 10);

    printf("Leader activo en puerto %d. Esperando hasta %d workers...\n", port, max_workers);

    socklen_t addrlen = sizeof(struct sockaddr_in);

    while (current_workers < max_workers)
    {
        int sock_servicio = accept(sock_escucha, (struct sockaddr *)&worker_addrs[current_workers], &addrlen);
        if (sock_servicio == -1)
        {
            perror("Error: accept");
            continue;
        }

        char pid_msg[64];
        int pid = 0;
        ssize_t bytes = read(sock_servicio, pid_msg, sizeof(pid_msg) - 1);
        if (bytes > 0)
        {
            pid_msg[bytes] = '\0';
            sscanf(pid_msg, "PID:%d", &pid);
        }

        worker_sockets[current_workers] = sock_servicio;
        worker_pids[current_workers] = pid;  
        current_workers++;

        char ip_str[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(worker_addrs[current_workers - 1].sin_addr), ip_str, INET_ADDRSTRLEN);

        printf("[OK] Worker aceptado desde %s [pid=%d] (%d/%d)\n",
           ip_str, pid, current_workers, max_workers);

        if (current_workers < max_workers)
        {
            printf("Esperando más workers... (o presione ENTER para iniciar)\n");
            fd_set readfds;
            FD_ZERO(&readfds);
            FD_SET(STDIN_FILENO, &readfds);
            struct timeval timeout = {2, 0};
            int r = select(STDIN_FILENO + 1, &readfds, NULL, NULL, &timeout);
            if (r > 0 && FD_ISSET(STDIN_FILENO, &readfds))
                break;
        }
    }

    printf("\n[OK] Iniciando distribución de tareas ...\n");
    long total_combinations = pow(36, relleno_len);
    long worker_range = total_combinations / current_workers;

    for (int i = 0; i < current_workers; i++)
    {
        long start = i * worker_range;
        long end = (i == current_workers - 1) ? total_combinations - 1 : start + worker_range - 1;

        char start_str[16], end_str[16];
        index_to_str(start, relleno_len, start_str);
        index_to_str(end, relleno_len, end_str);

        char instruction[512];
        sprintf(instruction, "TASK|minar|base=%s|len=%d|start=%s|end=%s|cond=%s\n", texto_base, relleno_len, start_str, end_str, condicion);

        write(worker_sockets[i], instruction, strlen(instruction));
        printf("Rango de busqueda enviado al worker %d: %s - %s\n", i + 1, start_str, end_str);

        if (fork() == 0)
        {
            close(sock_escucha);
            receive_worker(i,worker_sockets[i], worker_addrs[i]);
            exit(0);
        }
    }

    close(sock_escucha);

    while (current_workers > 0)
    {
        sleep(1);
    }

    printf("\nTodos los workers han finalizado. Cerrando leader...\n");

    return 0;
}