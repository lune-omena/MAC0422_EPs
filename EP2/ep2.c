/******************************************************************************************************
 * EP2 - CICLISTAS
 * 
 * Nome: Lara Ayumi Nagamatsu             NUSP: 9910568
 * Nome: Lucy Anne de Omena Evangelista   NUSP: 11221776
 * 
 *****************************************************************************************************/

/* Bibliotecas e Header */
#include "ep2.h"            /* header */
#include <stdio.h>          /* printf(), scanf(), ... */
#include <pthread.h>        /* POSIX threads */
#include <stdlib.h>         /* atoi()*/

/* Variáveis globais */
pthread_t ** pista;         /* representa a pista dos ciclistas, possui [d][10] casas */
 
int main(int argc, char * argv[]) {
    printf("EP2 - Ciclistas\n");

    /* Com relação à entrada, seu simulador deve receber como argumentos de linha de comando, nesta
       ordem, os dois números inteiros: d e n (i.e. vai rodar ./ep2 d n) */

    if(!argv[1]) {
        printf("Insira um comprimento de velódromo (d).\n");
        exit(EXIT_FAILURE);
    }
    else if(!argv[2]) {
        printf("Insira um número de ciclistas (n)\n");
        exit(EXIT_FAILURE);
    }
    
    int d = atoi(argv[1]);
    int n = atoi(argv[2]);
    
    if(d < 250) {
        printf("Insira um comprimento de velódromo (d) maior ou igual a 250.\n");
        exit(EXIT_FAILURE);
    }
    else if(n < 5 || n > 5*d ) {
        printf("Insira um número de ciclistas (n) maior que 5 e menor ou igual a (d*5).\n");
        exit(EXIT_FAILURE);
    }

    /* SIMULAÇÃO */

    /* a PISTA deve possuir 10 posições, mas no início da simulação apenas 5 estarão ocupadas */
    // lembrando que pthread_t = unsigned long int, estou zerando todas posições antes de ocupá-las
    pista = (pthread_t **) malloc(d*sizeof(pthread_t *)); //[d][10]

    for(int i = 0; i < d; i++) {
        pista[i] = (pthread_t *) malloc(10*sizeof(pthread_t));

        for(int j = 0; j < 10; j++) {
            pista[i][j] = 0;
        }
    }

    /* Seu simulador deve criar n threads “ciclista” iguais */
    pthread_t tid[n];

    // Assim que houver a "largada", os ciclistas serão criados:
    // Os ciclistas largam em fila ordenados aleatoriamente com no máximo 5 ciclistas 
    // lado a lado em cada posição.
    // obs: todos ciclistas começam com velocidade inicial 30Km/h (1m a cada 120ms)!!
    for(int i = 0; i < n; i++) {
        if (pthread_create(&tid[i], NULL, thread, NULL)) {
            printf("\n ERROR creating thread\n");
            exit(1);
        }
        //codigo associando a uma posição na pista, respeitando a condição de 5
        //ciclistas lado a lado
    }

    /* A cada duas voltas o ciclista que completar a última volta na última posição é eliminado.
       A prova termina quando sobrar apenas um ciclista, que é o campeão.
    */
    while(n > 1) {
        
    }

    /* Fim da execução */

    // liberando memória...
    for(int i = 0; i < d; i++)
        free(pista[i]);

    free(pista);

    return 0;
}

void * thread(void * a) {
    /* a thread vai ser criada e vai rodar este código yay*/
    //pthread_mutex_lock();
    printf("epa\n");
    //pthread_mutex_unlock();

    /* INICIALMENTE: */
    // Os ciclistas largam em fila ordenados aleatoriamente com no máximo 5 ciclistas 
    // lado a lado em cada posição.
    // obs: todos ciclistas começam com velocidade inicial 30Km/h (1m a cada 120ms)!!
    // pensando em criar variável para checar se deu a volta ou não -> teria q colocar d global

    /* LAÇO (provavelmente): */
    /* pthread_wait? -> sinal de passar tempo*/
    /* checa se precisa/dá pra ultrapassar outros amigos -> faz isso se possível com mutex */

    /* quando a chegar na outra rodada: laço? enquanto não chegou na rodada n roda isso */
    // checaria aqui a vairável se deu a volta ou não
    /* sortear nova velocidade */

    return NULL;
}