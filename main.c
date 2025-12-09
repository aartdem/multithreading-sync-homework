#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>
#include <unistd.h>

#define ROUNDS 10

static pthread_t ping_tid;
static pthread_t pong_tid;

/* Поток "ping": ждёт SIGUSR1, пишет "ping", будит pong SIGUSR2 */
void* ping_thread() {
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGUSR1);

    for (int i = 0; i < ROUNDS; ++i) {
        int sig;
        /* Ждём своего сигнала */
        if (sigwait(&set, &sig) != 0) {
            perror("sigwait ping");
            pthread_exit(NULL);
        }

        printf("ping\n");
        fflush(stdout);

        sleep(1);

        /* Будим pong */
        if (pthread_kill(pong_tid, SIGUSR2) != 0) {
            perror("pthread_kill to pong");
            pthread_exit(NULL);
        }
    }

    return NULL;
}

/* Поток "pong": ждёт SIGUSR2, пишет "pong", будит ping SIGUSR1 */
void* pong_thread() {
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGUSR2);

    for (int i = 0; i < ROUNDS; ++i) {
        int sig;
        if (sigwait(&set, &sig) != 0) {
            perror("sigwait pong");
            pthread_exit(NULL);
        }

        printf("pong\n");
        fflush(stdout);

        sleep(1);

        if (pthread_kill(ping_tid, SIGUSR1) != 0) {
            perror("pthread_kill to ping");
            pthread_exit(NULL);
        }
    }

    return NULL;
}

int main(void) {
    sigset_t block_set;

    sigemptyset(&block_set);
    sigaddset(&block_set, SIGUSR1);
    sigaddset(&block_set, SIGUSR2);

    if (pthread_sigmask(SIG_BLOCK, &block_set, NULL) != 0) {
        perror("pthread_sigmask");
        return EXIT_FAILURE;
    }

    /* Создаём потоки */
    if (pthread_create(&ping_tid, NULL, ping_thread, NULL) != 0) {
        perror("pthread_create ping");
        return EXIT_FAILURE;
    }

    if (pthread_create(&pong_tid, NULL, pong_thread, NULL) != 0) {
        perror("pthread_create pong");
        return EXIT_FAILURE;
    }

    if (pthread_kill(ping_tid, SIGUSR1) != 0) {
        perror("pthread_kill start");
        return EXIT_FAILURE;
    }

    /* Ждём окончания потоков */
    pthread_join(ping_tid, NULL);
    pthread_join(pong_tid, NULL);

    return EXIT_SUCCESS;
}
