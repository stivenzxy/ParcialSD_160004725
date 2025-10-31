#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/select.h>

#include "../include/socket_utils.h"
#include "../include/mining_utils.h"
#include "../include/string_utils.h"
#include "../include/worker.h"
#include <sys/socket.h>

void start_worker(const char *server_ip, int port)
{
    int sock = connect_to_server(server_ip, port);
    if (sock < 0)
    {
        fprintf(stderr, "Error al conectar con el líder.\n");
        exit(1);
    }

    char buffer[MAXLINE];
    ssize_t n = read(sock, buffer, MAXLINE - 1);
    if (n <= 0)
    {
        printf("[X] No se recibieron instrucciones.\n");
        close(sock);
        return;
    }

    buffer[n] = '\0';
    printf("Instrucción recibida: %s\n", buffer);

    char base[128], start[16], end[16], cond_raw[64], len_str[8];
    get_value(buffer, "base=", base, sizeof(base));
    get_value(buffer, "start=", start, sizeof(start));
    get_value(buffer, "end=", end, sizeof(end));
    get_value(buffer, "cond=", cond_raw, sizeof(cond_raw));
    get_value(buffer, "len=", len_str, sizeof(len_str));
    int len = atoi(len_str);

    printf("\nParámetros recibidos:\n  Start: %s\n  End: %s\n  Base: %s\n  Condición: %s\n  Longitud: %d\n",
           start, end, base, cond_raw, len);

    char *cond_type_str = strtok(cond_raw, ":");
    char *cond_value = strtok(NULL, ":");
    if (!cond_type_str || !cond_value)
    {
        printf("Error: formato de condición inválido\n");
        close(sock);
        return;
    }
    trim_str(cond_value);

    int cond_type = 0;
    if (strcmp(cond_type_str, "startswith") == 0)
        cond_type = 1;
    else if (strcmp(cond_type_str, "endswith") == 0)
        cond_type = 2;
    else if (strcmp(cond_type_str, "contains") == 0)
        cond_type = 3;

    printf("  Condición parseada: Tipo=%d, Valor=%s\n", cond_type, cond_value);
    printf("------------------------------------------------------\n");

    char combo[16];
    strncpy(combo, start, len);
    combo[len] = '\0';

    char full_string[256], hash[33];
    int found = 0;
    unsigned long iter = 0;
    int done = 0;

    fd_set readfds;
    struct timeval tv;

    while (!done)
    {
        FD_ZERO(&readfds);
        FD_SET(sock, &readfds);
        tv.tv_sec = 0;
        tv.tv_usec = 10000;
        int ready = select(sock + 1, &readfds, NULL, NULL, &tv);
        if (ready > 0 && FD_ISSET(sock, &readfds))
        {
            char cmd[64];
            ssize_t bytes = recv(sock, cmd, sizeof(cmd) - 1, MSG_DONTWAIT);
            if (bytes > 0)
            {
                cmd[bytes] = '\0';
                const char *message = "STOP";
                if (strncmp(cmd, message, strlen(message)) == 0)
                {
                    printf("Otro worker encontró el hash. Búsqueda detenida.\n");
                    break;
                }
            }
        }

        snprintf(full_string, sizeof(full_string), "%s%s", base, combo);
        md5_string(full_string, hash);

        if (hash_meets_condition(hash, cond_type, cond_value))
        {
            printf("¡Hash encontrado!: %s -> %s\n", full_string, hash);
            char msg[MAXLINE];
            snprintf(msg, sizeof(msg), "FOUND:%s:%s\n", full_string, hash);
            write(sock, msg, strlen(msg));
            found = 1;
            break;
        }

        iter++;
        if (iter % 1000 == 0)
        {
            printf("Progreso: combinación=%s iteración=%lu\n", combo, iter);
            fflush(stdout);
        }

        if (strcmp(combo, end) == 0)
            done = 1;
        else
            next_combination(combo, len);
    }

    if (!found)
    {
        const char *message = "NOTFOUND\n";
        write(sock, message, strlen(message));
        printf("Hash no encontrado dentro del rango.\n");
    }

    close(sock);
}
