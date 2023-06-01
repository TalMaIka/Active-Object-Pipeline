// st_pipeline will get one or two argumets from the command line
// the first will be N that will indicate the number of the tasks and the second will be a random seed
// the program will create N tasks and will connect them in a pipeline

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <string.h>

typedef struct Queue
{
    void **ptr;
    int size;
    int front;
    int rear;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
} Queue;

Queue *queueInit()
{
    Queue *q = malloc(sizeof(Queue));
    q->ptr = malloc(sizeof(void *) * 5);
    q->size = 0;
    q->front = 0;
    q->rear = 0;
    pthread_mutex_init(&q->mutex, NULL);
    pthread_cond_init(&q->cond, NULL);
    return q;
}

void queueDelete(Queue *q)
{
    pthread_mutex_destroy(&q->mutex);
    pthread_cond_destroy(&q->cond);
    free(q->ptr);
    free(q);
}

void queueEnqueue(Queue *q, void *in)
{
    pthread_mutex_lock(&q->mutex);
    if (q->size == q->rear)
    {
        q->ptr = realloc(q->ptr, sizeof(void *) * (q->size + 1));
    }
    q->ptr[q->rear] = in;
    q->rear++;
    q->size++;
    pthread_cond_signal(&q->cond);
    printf("Signalled\n");
    pthread_mutex_unlock(&q->mutex);
}

void *queueDequeue(Queue *q)
{
    pthread_mutex_lock(&q->mutex);
    while (q->size == 0)
    {
        printf("Waiting for cond signal...\n");
        pthread_cond_wait(&q->cond, &q->mutex);
    }
    void *out = q->ptr[q->front];
    memmove(&q->ptr[q->front], &q->ptr[q->front + 1], sizeof(void *) * (q->rear - q->front - 1));
    q->rear--;
    q->size--;
    pthread_mutex_unlock(&q->mutex);
    return out;
}

typedef struct ActiveObject
{
    pthread_t thread;
    Queue *queue;
    int running;
    void (*func)(void *);
} ActiveObject;

int isPrime(int num)
{
    if (num <= 1)
        return 0;
    if (num <= 3)
        return 1;
    if (num % 2 == 0 || num % 3 == 0)
        return 0;
    for (int i = 5; i * i <= num; i += 6)
    {
        if (num % i == 0 || num % (i + 2) == 0)
            return 0;
    }
    return 1;
}

void isPrimeAdapter(void *arg)
{
    int *num = (int *)arg;
    int result = isPrime(*num);
    printf("%d is %s\n", *num, result ? "prime" : "not prime");
    free(num);
}

void *runActiveObject(void *arg)
{
    ActiveObject *ao = (ActiveObject *)arg;
    void *task;
    while (ao->running && (task = queueDequeue(ao->queue)))
    {
        ao->func(task);
    }
    return NULL;
}

ActiveObject *createActiveObject(void (*func)(void *))
{
    ActiveObject *ao = (ActiveObject *)malloc(sizeof(ActiveObject));
    ao->queue = queueInit();
    ao->func = func;
    ao->running = 1;
    pthread_create(&ao->thread, NULL, runActiveObject, (void *)ao);
    return ao;
}

void stop(ActiveObject *ao)
{
    ao->running = 0;
    pthread_join(ao->thread, NULL);
    queueDelete(ao->queue);
    free(ao);
}

Queue *GetQueue(ActiveObject *ao, void *ptr)
{
    if (ptr != NULL)
    {
        queueEnqueue(ao->queue, ptr);
    }
    return ao->queue;
}

ActiveObject *AO1;
ActiveObject *AO2;
ActiveObject *AO3;
ActiveObject *AO4;

void AO1_func(void *arg)
{
    int *data = (int *)arg;
    *data = rand() % 900000 + 100000; // generate a 6-digit number
    queueEnqueue(AO2->queue, data);   // pass it to the next AO
}

void AO2_func(void *arg)
{
    int *data = (int *)arg;
    printf("%d is %s\n", *data, isPrime(*data) ? "prime" : "not prime");
    *data += 11;
    queueEnqueue(AO3->queue, data); // pass it to the next AO
}

void AO3_func(void *arg)
{
    int *data = (int *)arg;
    printf("%d is %s\n", *data, isPrime(*data) ? "prime" : "not prime");
    *data -= 13;
    queueEnqueue(AO4->queue, data); // pass it to the next AO
}

void AO4_func(void *arg)
{
    int *data = (int *)arg;
    printf("%d\n", *data);
    *data += 2;
    printf("%d\n", *data);
    free(data); //  free the data
}


int main(int argc, char *argv[])
{
    int N = atoi(argv[1]);                                                     // number of tasks
    unsigned int seed = (argc > 2) ? atoi(argv[2]) : (unsigned int)time(NULL); // random seed
    srand(seed);

    // create the AOs
    ActiveObject *AO1 = createActiveObject(AO1_func);
    ActiveObject *AO2 = createActiveObject(AO2_func);
    ActiveObject *AO3 = createActiveObject(AO3_func);
    ActiveObject *AO4 = createActiveObject(AO4_func);

    // run N tasks
    for (int i = 0; i < N; i++)
    {
        int *data = malloc(sizeof(int));
        queueEnqueue(AO1->queue, data); // start a task in AO1
        sleep(1);                       // wait 1 second between tasks
    }

    // wait for all tasks to finish...
    // (you might want to add some mechanism to signal the end of tasks and make AOs stop)

    // clean up
    stop(AO1);
    stop(AO2);
    stop(AO3);
    stop(AO4);

    return 0;
}
// int main(int argc, char *argv[]){
//     int N = 0;
//     int seed = 0;
//     if(argc > 3){
//         printf("Wrong number of arguments\n");
//         return 1;
//     }
//     if(argc == 2){
//         N = atoi(argv[1]);
//         seed = rand() % 900000 + 100000;
//     }
//     if(argc == 3){
//         N = atoi(argv[1]);
//         seed = atoi(argv[2]);
//     }

//     ActiveObject* FirstAO = createActiveObject(isPrimeAdapter);

//     for (int i = 0; i <= N; i++) {
//         int *data = malloc(sizeof(int));
//         *data = i;
//         queueEnqueue(FirstAO->queue, generateRandomNumbers(N, seed));
//         sleep(1); // Simulate some processing time
//     }

//     // You might want to add some mechanism to wait for all tasks to finish processing before calling stop.
//     stop(FirstAO);

//     return 0;
// }