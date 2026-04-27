#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <time.h>

#define MAX 100000   // M = 10^5

int *buffer;
int N;              // tamanho do buffer
int in = 0, out = 0;
int count = 0;

sem_t empty, full, mutex;

int is_prime(int num) {
    if (num < 2) return 0;
    for (int i = 2; i * i <= num; i++) {
        if (num % i == 0) return 0;
    }
    return 1;
}

void* producer(void* arg) {
    while (1) {
        int num = rand() % 10000000 + 1;

        sem_wait(&empty);
        sem_wait(&mutex);

        buffer[in] = num;
        in = (in + 1) % N;

        sem_post(&mutex);
        sem_post(&full);
    }
    return NULL;
}

void* consumer(void* arg) {
    while (1) {
        sem_wait(&full);
        sem_wait(&mutex);

        int num = buffer[out];
        out = (out + 1) % N;
        count++;

        sem_post(&mutex);
        sem_post(&empty);

        if (is_prime(num)) {
            printf("%d é primo\n", num);
        }

        if (count >= MAX) {
            exit(0);
        }
    }
    return NULL;
}

int main(int argc, char* argv[]) {
    if (argc < 4) {
        printf("Uso: %s <N> <Np> <Nc>\n", argv[0]);
        return 1;
    }

    N = atoi(argv[1]);
    int Np = atoi(argv[2]);
    int Nc = atoi(argv[3]);

    buffer = malloc(N * sizeof(int));
    srand(time(NULL));

    sem_init(&empty, 0, N);
    sem_init(&full, 0, 0);
    sem_init(&mutex, 0, 1);

    pthread_t prod[Np], cons[Nc];

    for (int i = 0; i < Np; i++) pthread_create(&prod[i], NULL, producer, NULL);
    for (int i = 0; i < Nc; i++) pthread_create(&cons[i], NULL, consumer, NULL);

    for (int i = 0; i < Np; i++) pthread_join(prod[i], NULL);
    for (int i = 0; i < Nc; i++) pthread_join(cons[i], NULL);

    free(buffer);
    return 0;
}
