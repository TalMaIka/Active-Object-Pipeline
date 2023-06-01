// st_pipeline will get one or two argumets from the command line
// the first will be N that will indicate the number of the tasks and the second will be a random seed
// the program will create N tasks and will connect them in a pipeline

#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

typedef struct Queue {
    void** ptr;
    int size;
    int front;
    int rear;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
} Queue;

struct ActiveObject {
    pthread_t thread;
    Queue *queue;
    int running;
    void (*run)(void *);
    void *arg;
};


int main(int argc, char *argv[]){
    if(argc > 3){
        printf("Wrong number of arguments\n");
        return 1;
    }
    if(argc == 2){
        int N = atoi(argv[1]);
        int seed = time(NULL);
    }
    if(argc == 3){
        int N = atoi(argv[1]);
        int seed = atoi(argv[2]);
    }
    
    return 0;
}