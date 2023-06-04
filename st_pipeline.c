#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <string.h>
#include "Queue.h"
#include "ActiveObject.h"
#include "Prime.h"

ActiveObject *AO1;
ActiveObject *AO2;
ActiveObject *AO3;
ActiveObject *AO4;

int Ncounter = 1;
int N;

void AO1_func(void *arg)
{
    int *data = (int *)arg;
    *data = rand() % 900000 + 100000; 
    queueEnqueue(AO2->queue, data);
    printf("%d\n", *data);
}

void AO2_func(void *arg)
{
    int *data = (int *)arg;
    printf(isPrime(*data) ? "true\n" : "false\n");
    *data += 11;
    printf("%d\n", *data);
    queueEnqueue(AO3->queue, data); 
}

void AO3_func(void *arg)
{
    int *data = (int *)arg;
    printf(isPrime(*data) ? "true\n" : "false\n");
    *data -= 13;
    printf("%d\n", *data);
    queueEnqueue(AO4->queue, data); 
}

void AO4_func(void *arg)
{
    int *data = (int *)arg;
    printf(isPrime(*data) ? "true\n" : "false\n");
    *data += 2;
    printf("%d\n", *data);
    free(data);
    if(Ncounter == N){
        stop(AO1);
        stop(AO2);
        stop(AO3);
        stop(AO4);              
    }
}

int main(int argc, char *argv[])
{
    N = atoi(argv[1]);                                                     
    unsigned int seed = (argc > 2) ? atoi(argv[2]) : (unsigned int)time(NULL);
    srand(seed);

    AO1 = createActiveObject(AO1_func);
    AO2 = createActiveObject(AO2_func);
    AO3 = createActiveObject(AO3_func);
    AO4 = createActiveObject(AO4_func);

    for (int i = 0; i < N; i++)
    {
        int *data = (int*)malloc(sizeof(int));
        queueEnqueue(AO1->queue, data);
        sleep(1);  
        Ncounter++;
    }

    return 0;
}
