#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <string.h>

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


typedef struct ActiveObject {
    pthread_t thread;
    Queue* queue;
    int running;
    void (*func)(void*);
} ActiveObject;

int isPrime(int num) {
    if (num <= 1) return 0;
    if (num <= 3) return 1;
    if (num % 2 == 0 || num % 3 == 0) return 0;
    for (int i = 5; i * i <= num; i += 6) {
        if (num % i == 0 || num % (i + 2) == 0) return 0;
    }
    return 1;
}

void isPrimeAdapter(void* arg) {
    int* num = (int*)arg;
    int result = isPrime(*num);
    printf("%d is %s\n", *num, result ? "prime" : "not prime");
    free(num);
}

void* runActiveObject(void* arg) {
    ActiveObject* ao = (ActiveObject*)arg;
    void* task;
    while (ao->running && (task = queueDequeue(ao->queue))) {
        ao->func(task);
    }
    return NULL;
}

ActiveObject* createActiveObject(void (*func)(void*)) {
    ActiveObject* ao = (ActiveObject*)malloc(sizeof(ActiveObject));
    ao->queue = queueInit();
    ao->func = func;
    ao->running = 1;
    pthread_create(&ao->thread, NULL, runActiveObject, (void*)ao);
    return ao;
}

void stop(ActiveObject* ao) {
    ao->running = 0;
    pthread_join(ao->thread, NULL);
    queueDelete(ao->queue);
    free(ao);
}

Queue* GetQueue(ActiveObject* ao, void* ptr){
    if(ptr != NULL){
        queueEnqueue(ao->queue, ptr);
    }
    return ao->queue;
}


int main() {
    ActiveObject* ao = createActiveObject(isPrimeAdapter);

    for (int i = 0; i <= 20; i++) {
        int *data = malloc(sizeof(int));
        *data = i;
        queueEnqueue(ao->queue, data);
        sleep(1); // Simulate some processing time
    }

    // You might want to add some mechanism to wait for all tasks to finish processing before calling stop.
    stop(ao);

    return 0;
}