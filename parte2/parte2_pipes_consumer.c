#include <errno.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MSG_SIZE 20

/* Le exatamente count bytes, tratando interrupcoes e EOF parcial. */
static ssize_t read_full(int fd, void *buf, size_t count) {
    char *p = (char *)buf;
    size_t total = 0;

    while (total < count) {
        ssize_t n = read(fd, p + total, count - total);
        if (n < 0) {
            if (errno == EINTR) {
                continue;
            }
            return -1;
        }
        if (n == 0) {
            break;
        }
        total += (size_t)n;
    }

    return (ssize_t)total;
}

/* Recebe uma mensagem fixa de 20 bytes e converte para inteiro. */
static int receive_number(int fd, unsigned long long *value) {
    char msg[MSG_SIZE];
    ssize_t n = read_full(fd, msg, sizeof(msg));

    if (n == 0) {
        return 1;
    }
    if (n < 0) {
        return -1;
    }
    if (n != (ssize_t)sizeof(msg)) {
        errno = EPROTO;
        return -1;
    }

    msg[sizeof(msg) - 1] = '\0';

    errno = 0;
    char *endptr = NULL;
    unsigned long long parsed = strtoull(msg, &endptr, 10);
    if (errno != 0 || endptr == msg) {
        errno = EPROTO;
        return -1;
    }

    *value = parsed;
    return 0;
}

/* Teste de primalidade por tentativa ate sqrt(n), pulando pares. */
static bool is_prime_ull(unsigned long long n) {
    if (n < 2) {
        return false;
    }
    if (n == 2) {
        return true;
    }
    if ((n % 2ULL) == 0ULL) {
        return false;
    }

    unsigned long long limit = (unsigned long long)sqrt((long double)n);
    for (unsigned long long i = 3ULL; i <= limit; i += 2ULL) {
        if ((n % i) == 0ULL) {
            return false;
        }
    }

    return true;
}

int consumer_loop(int read_fd) {
    for (;;) {
        unsigned long long numero = 0ULL;
        int rc = receive_number(read_fd, &numero);
        if (rc == 1) {
            return 0;
        }
        if (rc < 0) {
            perror("read");
            return 1;
        }

        /* Sentinela de fim de fluxo enviada pelo produtor. */
        if (numero == 0ULL) {
            return 0;
        }

        printf("Consumidor: %llu -> %s\n", numero, is_prime_ull(numero) ? "primo" : "nao primo");
    }
}
