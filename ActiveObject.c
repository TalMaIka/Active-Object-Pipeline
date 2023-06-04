#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <string.h>
#include "Queue.h"
#include "ActiveObject.h"

void* runActiveObject(void* arg) {
    ActiveObject* ao = (ActiveObject*)arg;
    void *task;
    while (ao->running) {
        pthread_mutex_lock(&ao->mutex);
        task = queueDequeue(ao->queue);
        pthread_mutex_unlock(&ao->mutex);
        if(task == (void*)-1){
            break;
        }
        ao->func(task);
    }
    return NULL;
}

ActiveObject* createActiveObject(void (*func)(void*)) {
    ActiveObject* ao = (ActiveObject*)malloc(sizeof(ActiveObject));
    ao->queue = queueInit();
    ao->func = func;
    ao->running = 1;
    pthread_mutex_init(&ao->mutex, NULL);
    pthread_create(&ao->thread, NULL, runActiveObject, (void*)ao);
    return ao;
}

void stop(ActiveObject* ao) {
    ao->running = 0;
    pthread_join(ao->thread, NULL);
    queueDelete(ao->queue);
    pthread_mutex_destroy(&ao->mutex);
    free(ao);
}

Queue *GetQueue(ActiveObject* ao, void* ptr){
    pthread_mutex_lock(&ao->mutex);
    if(ptr != NULL){
        queueEnqueue(ao->queue, ptr);
    }
    pthread_mutex_unlock(&ao->mutex);
    return ao->queue;
}
