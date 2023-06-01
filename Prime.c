#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int isPrime(unsigned int n) {
    unsigned int i;
    if (n < 2) return 0;
    int j = 1;
    for (i = 2; i <= sqrt(n); i+=(2*j+1)) {
        if (n % i == 0){
            return 0;
        } 
        j++;
    }
    return 1;
}