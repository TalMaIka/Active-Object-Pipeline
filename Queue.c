#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

typedef struct Queue {
    void** ptr;
    int size;
    int front;
    int rear;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
} Queue;

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
    printf("Signalled\n");
    pthread_mutex_unlock(&q->mutex);
}

void* queueDequeue(Queue* q) {
    pthread_mutex_lock(&q->mutex);
    while (q->size == 0) {
        printf("Waiting for cond signal...\n");
        pthread_cond_wait(&q->cond, &q->mutex);
    }
    void* out = q->ptr[q->front];
    memmove(&q->ptr[q->front], &q->ptr[q->front + 1], sizeof(void*) * (q->rear - q->front - 1));
    q->rear--;
    q->size--;
    pthread_mutex_unlock(&q->mutex);
    return out;
}

void* producer(void* arg) {
    Queue* q = (Queue*)arg;
    int i;
    for (i = 0; i < 20; i++) {
        int* data = malloc(sizeof(int));
        *data = i;
        queueEnqueue(q, data);
        printf("Produced: %d\n", *data);
        sleep(1);
    }
    return NULL;
}

void* consumer(void* arg) {
    Queue* q = (Queue*)arg;
    int i;
    for (i = 0; i < 20; i++) {
        int* data = (int*)queueDequeue(q);
        printf("Consumed: %d\n", *data);
        free(data);
        sleep(1);
    }
    return NULL;
}

int main() {
    Queue* q = queueInit();

    pthread_t producerThread, consumerThread;

    if (pthread_create(&producerThread, NULL, producer, (void*)q) != 0) {
        fprintf(stderr, "Failed to create producer thread\n");
        return 1;
    }

    if (pthread_create(&consumerThread, NULL, consumer, (void*)q) != 0) {
        fprintf(stderr, "Failed to create consumer thread\n");
        return 1;
    }

    pthread_join(producerThread, NULL);
    pthread_join(consumerThread, NULL);

    queueDelete(q);

    return 0;
}