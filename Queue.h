#ifndef QUEUE_H
#define QUEUE_H

typedef struct Queue {
    void** ptr;
    int size;
    int front;
    int rear;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
} Queue;


Queue* queueInit();
void queueDelete(Queue* q);
void queueEnqueue(Queue* q, void* in);
void* queueDequeue(Queue* q);

#endif