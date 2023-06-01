#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>



typedef struct ActiveObject {
    pthread_t thread;
    Queue *queue;
    int running;
    void (*func)(void *);
    void *arg;
} ActiveObject;

//CreateActiveObject() that will take a pointer to a function and a pointer to an argument
//and create an ActiveObject that will run the function with the argument
//when the ActiveObject is started.
void CreateActiveObject(void (*func)(void *)){
    ActiveObject *ao = malloc(sizeof(ActiveObject));
    ao->queue = queueInit(10);
    ao->running = 1;
    ao->func = func;
    ao->arg = NULL;
    while(ao->running && ao->queue->size > 0){
        void *arg = queueDequeue(ao->queue);
        //pthread_create(&ao->thread, NULL, &func, arg);
        ao->func(arg);
    }
}

//GetQueue will return a pointer to the queue of the ActiveObject
Queue *GetQueue(ActiveObject *ao){
    return ao->queue;
}

//stop() will stop the ActiveObject
void stop(ActiveObject *ao){
    ao->running = 0;
    queueDelete(ao->queue);
    free(ao);
}

void *run(void *arg){
    ActiveObject *ao = (ActiveObject *)arg;
    while(ao->running){
        void *arg = queueDequeue(ao->queue);
        ao->func(arg);
    }
    return NULL;
}


int main(void){
    ActiveObject *ao = malloc(sizeof(ActiveObject));
    ao->queue = queueInit(10);
    ao->running = 1;
    ao->func = run;
    ao->arg = NULL;
    pthread_create(&ao->thread, NULL, &run, ao);
    int i;
    for(i = 0; i < 5; i++){
        int *data = malloc(sizeof(int));
        *data = i;
        queueEnqueue(ao->queue, data);
    }
    sleep(1);
    stop(ao);
    return 0;
}