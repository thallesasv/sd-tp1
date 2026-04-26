/*
 * Parte 2 - Pipes (Produtor x Consumidor)
 *
 * Versao com arquivos separados:
 * - parte2_pipes_producer.c: cria pipe/fork e gera os numeros.
 * - parte2_pipes_consumer.c: implementa o loop do consumidor.
 *
 * Compilacao:
 *   gcc -O2 -Wall -Wextra -o parte2_pipes_split parte2_pipes_producer.c parte2_pipes_consumer.c -lm
 *
 * Execucao:
 *   ./parte2_pipes_split <quantidade>
 */

#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define MSG_SIZE 20
#define DELTA_MIN 1
#define DELTA_MAX 100

int consumer_loop(int read_fd);

/* Garantir escrita completa de cada mensagem fixa no pipe. */
static ssize_t write_full(int fd, const void *buf, size_t count) {
    const char *p = (const char *)buf;
    size_t total = 0;

    while (total < count) {
        ssize_t written = write(fd, p + total, count - total);
        if (written < 0) {
            if (errno == EINTR) {
                continue;
            }
            return -1;
        }
        total += (size_t)written;
    }

    return (ssize_t)total;
}

static double elapsed_seconds(const struct timespec *start, const struct timespec *end) {
    time_t sec = end->tv_sec - start->tv_sec;
    long nsec = end->tv_nsec - start->tv_nsec;

    if (nsec < 0) {
        --sec;
        nsec += 1000000000L;
    }

    return (double)sec + (double)nsec / 1000000000.0;
}

/* Serializa cada numero em exatamente 20 bytes (19 digitos + '\0'). */
static int send_number(int fd, unsigned long long value) {
    char msg[MSG_SIZE];
    int rc = snprintf(msg, sizeof(msg), "%019llu", value);
    if (rc < 0 || rc >= (int)sizeof(msg)) {
        return -1;
    }

    return (write_full(fd, msg, sizeof(msg)) == (ssize_t)sizeof(msg)) ? 0 : -1;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <quantidade>\n", argv[0]);
        return EXIT_FAILURE;
    }

    errno = 0;
    char *endptr = NULL;
    unsigned long long quantidade = strtoull(argv[1], &endptr, 10);
    if (errno != 0 || endptr == argv[1] || *endptr != '\0') {
        fprintf(stderr, "Parametro invalido: '%s'\n", argv[1]);
        return EXIT_FAILURE;
    }

    int pipefd[2];
    if (pipe(pipefd) < 0) {
        perror("pipe");
        return EXIT_FAILURE;
    }

    struct timespec start_time;
    if (clock_gettime(CLOCK_MONOTONIC, &start_time) < 0) {
        perror("clock_gettime");
        close(pipefd[0]);
        close(pipefd[1]);
        return EXIT_FAILURE;
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        close(pipefd[0]);
        close(pipefd[1]);
        return EXIT_FAILURE;
    }

    if (pid == 0) {
        close(pipefd[1]);
        int rc = consumer_loop(pipefd[0]);
        close(pipefd[0]);
        exit(rc == 0 ? EXIT_SUCCESS : EXIT_FAILURE);
    }

    close(pipefd[0]);

    unsigned long long atual = 1ULL;
    unsigned int seed = (unsigned int)(time(NULL) ^ (unsigned int)getpid());

    /* O protocolo usa 0 como sentinela, entao envia N0=1 quando quantidade > 0. */
    if (quantidade > 0ULL) {
        if (send_number(pipefd[1], atual) < 0) {
            perror("write");
            close(pipefd[1]);
            waitpid(pid, NULL, 0);
            return EXIT_FAILURE;
        }
    }

    for (unsigned long long i = 1ULL; i < quantidade; ++i) {
        unsigned long long delta = (unsigned long long)((rand_r(&seed) % DELTA_MAX) + DELTA_MIN);

        if (ULLONG_MAX - atual < delta) {
            fprintf(stderr, "Overflow detectado ao gerar sequencia\n");
            close(pipefd[1]);
            waitpid(pid, NULL, 0);
            return EXIT_FAILURE;
        }

        atual += delta;

        if (send_number(pipefd[1], atual) < 0) {
            perror("write");
            close(pipefd[1]);
            waitpid(pid, NULL, 0);
            return EXIT_FAILURE;
        }
    }

    if (send_number(pipefd[1], 0ULL) < 0) {
        perror("write");
        close(pipefd[1]);
        waitpid(pid, NULL, 0);
        return EXIT_FAILURE;
    }

    close(pipefd[1]);

    int status = 0;
    if (waitpid(pid, &status, 0) < 0) {
        perror("waitpid");
        return EXIT_FAILURE;
    }

    if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
        fprintf(stderr, "Consumidor terminou com erro\n");
        return EXIT_FAILURE;
    }

    struct timespec end_time;
    if (clock_gettime(CLOCK_MONOTONIC, &end_time) < 0) {
        perror("clock_gettime");
        return EXIT_FAILURE;
    }

    /* Tempo total inclui geracao, IPC e processamento no consumidor. */
    printf("Tempo total de execucao: %.6f s\n", elapsed_seconds(&start_time, &end_time));

    return EXIT_SUCCESS;
}
