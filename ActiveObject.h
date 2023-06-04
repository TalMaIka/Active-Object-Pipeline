#ifndef ACTIVEOBJECT_H
#define ACTIVEOBJECT_H


typedef struct ActiveObject {
    pthread_t thread;
    Queue* queue;
    int running;
    void (*func)(void*);
    pthread_mutex_t mutex;
} ActiveObject;

ActiveObject* createActiveObject(void (*func)(void*));
void stop(ActiveObject* ao);
Queue* GetQueue(ActiveObject* ao, void* ptr);
void* runActiveObject(void* arg);


#endif