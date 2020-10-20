/* Baseado em código de 
 * http://www.csc.villanova.edu/~mdamian/threads/posixsem.html 
 * e no tutorial em e no tutorial em https://computing.llnl.gov/tutorials/pthreads/#Mutexes */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define NITER 1000000000
#define VEZES 5

long int count1=0;
long int count2=0;

pthread_mutex_t mutex1;
pthread_mutex_t mutex2;

void * ThreadAdd1 (void * a) {
    long int i;
    
    for (i=0; i<(long int)NITER; i++) {
        pthread_mutex_lock(&mutex1);
        count1++;
        pthread_mutex_unlock(&mutex1);
    }

    return NULL;
}

void * ThreadAdd2 (void * a) {
    long int i;
    
    for (i=0; i<(long int)NITER; i++) {
        pthread_mutex_lock(&mutex2);
        count2++;
        pthread_mutex_unlock(&mutex2);
    }

    return NULL;
}

int main() {
    int i;
    pthread_t tid[2*VEZES];

    pthread_mutex_init(&mutex1, NULL);
    pthread_mutex_init(&mutex2, NULL);

    for (i=0; i<VEZES; i++) {
        if (pthread_create(&tid[i], NULL, ThreadAdd1, NULL)) {
            printf("\n ERROR creating thread 1");
            exit(1);
        }
        if (pthread_create(&tid[i+VEZES], NULL, ThreadAdd2, NULL)) {
            printf("\n ERROR creating thread 2");
            exit(1);
        }
    }
    
    for (i=0; i<2*VEZES; i++)   
        if (pthread_join(tid[i], NULL))  {
            printf("\n ERROR joining thread");
            exit(1);
        }

    if (count1<(long int)VEZES*NITER) 
        printf("\n OXE! a variavel count1 eh [%li], mas devia ser %li\n", count1, (long int) VEZES*NITER);
    else
        printf("\n Ufa! a variavel count1 eh [%li]\n", count1);

    if (count2<(long int)VEZES*NITER) 
        printf("\n OXE! a variavel count2 eh [%li], mas devia ser %li\n", count2, (long int) VEZES*NITER);
    else
        printf("\n Ufa! a variavel count2 eh [%li]\n", count2);

    pthread_mutex_destroy(&mutex1);
    pthread_mutex_destroy(&mutex2);

    return(0);
}
