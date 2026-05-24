#include "include/eduos.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define THREAD_POOL_SIZE 4

static int shared_counter = 0;
static pthread_mutex_t counter_mutex = PTHREAD_MUTEX_INITIALIZER;

static pthread_t thread_pool[THREAD_POOL_SIZE];
static pthread_mutex_t queue_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t queue_cond = PTHREAD_COND_INITIALIZER;
static int shutdown_pool = 0;

static void *worker_thread(void *arg) {
    while (1) {
        pthread_mutex_lock(&queue_mutex);
        while (!shutdown_pool) {
            pthread_cond_wait(&queue_cond, &queue_mutex);
            break;
        }
        if (shutdown_pool) {
            pthread_mutex_unlock(&queue_mutex);
            break;
        }
        pthread_mutex_unlock(&queue_mutex);
    }
    return NULL;
}

void init_thread_pool() {
    for (int i = 0; i < THREAD_POOL_SIZE; i++) {
        pthread_create(&thread_pool[i], NULL, worker_thread, NULL);
        printf("[ThreadPool] Worker thread %d created\n", i);
    }
}

void shutdown_thread_pool() {
    pthread_mutex_lock(&queue_mutex);
    shutdown_pool = 1;
    pthread_cond_broadcast(&queue_cond);
    pthread_mutex_unlock(&queue_mutex);
    for (int i = 0; i < THREAD_POOL_SIZE; i++) {
        pthread_join(thread_pool[i], NULL);
    }
    printf("[ThreadPool] All threads shut down\n");
}

void *increment_no_mutex(void *arg) {
    for (int i = 0; i < 10000; i++) {
        shared_counter++;
    }
    return NULL;
}

void *increment_with_mutex(void *arg) {
    for (int i = 0; i < 10000; i++) {
        pthread_mutex_lock(&counter_mutex);
        shared_counter++;
        pthread_mutex_unlock(&counter_mutex);
    }
    return NULL;
}

void reset_counter() { shared_counter = 0; }
int get_counter() { return shared_counter; }

void demo_race_condition() {
    printf("\n=== Race Condition Demo (NO mutex) ===\n");
    reset_counter();
    pthread_t t1, t2, t3, t4;
    pthread_create(&t1, NULL, increment_no_mutex, NULL);
    pthread_create(&t2, NULL, increment_no_mutex, NULL);
    pthread_create(&t3, NULL, increment_no_mutex, NULL);
    pthread_create(&t4, NULL, increment_no_mutex, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    pthread_join(t3, NULL);
    pthread_join(t4, NULL);
    printf("Expected: 40000, Got: %d\n", shared_counter);
    printf("Result is non-deterministic!\n");
}

void demo_fixed() {
    printf("\n=== Fixed Demo (WITH mutex) ===\n");
    reset_counter();
    pthread_t t1, t2, t3, t4;
    pthread_create(&t1, NULL, increment_with_mutex, NULL);
    pthread_create(&t2, NULL, increment_with_mutex, NULL);
    pthread_create(&t3, NULL, increment_with_mutex, NULL);
    pthread_create(&t4, NULL, increment_with_mutex, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    pthread_join(t3, NULL);
    pthread_join(t4, NULL);
    printf("Expected: 40000, Got: %d\n", shared_counter);
    printf("Result is always correct!\n");
}
