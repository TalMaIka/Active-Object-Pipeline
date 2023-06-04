#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include "Queue.h"

Queue* queueInit() {
    Queue* q = malloc(sizeof(Queue));
    q->ptr = malloc(sizeof(void*) * 5);
    q->size = 0;
    q->front = 0;
    q->rear = 0;
    pthread_mutex_init(&q->mutex, NULL);
    pthread_cond_init(&q->cond, NULL);
    return q;
}

void queueDelete(Queue* q) {
    pthread_mutex_destroy(&q->mutex);
    pthread_cond_destroy(&q->cond);
    free(q->ptr);
    free(q);
}

void queueEnqueue(Queue* q, void* in) {
    pthread_mutex_lock(&q->mutex);
    if (q->size == q->rear) {
        q->ptr = realloc(q->ptr, sizeof(void*) * (q->size+1));
    }
    q->ptr[q->rear] = in;
    q->rear++;
    q->size++;
    pthread_cond_signal(&q->cond);
    pthread_mutex_unlock(&q->mutex);
}

void* queueDequeue(Queue* q) {
    pthread_mutex_lock(&q->mutex);
    while (q->size == 0) {
        pthread_cond_wait(&q->cond, &q->mutex);
    }
    void* out = q->ptr[q->front];
    memmove(&q->ptr[q->front], &q->ptr[q->front + 1], sizeof(void*) * (q->rear - q->front - 1));
    q->rear--;
    q->size--;
    pthread_mutex_unlock(&q->mutex);
    return out;
}