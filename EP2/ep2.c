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
#include <unistd.h>         /* sleep()*/

/* Variáveis globais */
pthread_t ** pista;                 /* representa a pista dos ciclistas, possui [d][10] casas */
pthread_mutex_t mutex_main;         /* mutex usado para o escalonador */
pthread_cond_t wait_thread;         /* barreira (cond) para threads */
pthread_mutex_t mutex;
int volta = -1;                     /* número de voltas */
int total = -1;                     /* variável para que a main espere todas threads */
int ind_full = 0;                   /* índice da pista que se encontra "cheio" */
int tam_pista = 0;                  /* é igual a d */
 
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
    tam_pista = d;
    
    if(d < 250) {
        printf("Insira um comprimento de velódromo (d) maior ou igual a 250.\n");
        exit(EXIT_FAILURE);
    }
    else if(n < 5 || n > 5*d ) {
        printf("Insira um número de ciclistas (n) maior que 5 e menor ou igual a (d*5).\n");
        exit(EXIT_FAILURE);
    }

    /* SIMULAÇÃO */

    pthread_mutex_init(&mutex_main, NULL);
    pthread_mutex_init(&mutex, NULL);
    
    // número de voltas na simulação
    volta = 5;

    // todas threads precisam rodar
    total = 0;

    /* A pista deve possuir 10 posições para cada d, mas no início da simulação apenas 5 estarão ocupadas */
    // lembrando que pthread_t = unsigned long int, estou zerando todas posições antes de ocupá-las
    pista = (pthread_t **) malloc(d*sizeof(pthread_t *)); //[d][10]

    for(int i = 0; i < d; i++) {
        pista[i] = (pthread_t *) malloc(10*sizeof(pthread_t));

        for(int j = 0; j < 10; j++) {
            pista[i][j] = 0;
        }
    }

    /* Seu simulador deve criar n threads “ciclista” iguais */
    // portanto, devo alocar n mutex (para cada thread)
    pthread_t tid[n];

    // Assim que houver a "largada", os ciclistas serão criados:
    for(int i = 0; i < n; i++)
        if (pthread_create(&tid[i], NULL, thread, NULL)) {
            printf("\n ERROR creating thread\n");
            exit(1);
        }

    /* A cada duas voltas o ciclista que completar a última volta na última posição é eliminado.
       A prova termina quando sobrar apenas um ciclista, que é o campeão.
    */  //while(n > 1)

    for(int j = 0; j < 5; ) { // Simulação com 5 "voltas" (1 volta = 1 segundo)
        if(total == n) {
            pthread_mutex_lock(&mutex_main);
            sleep(1);
            printf("...\n");
            total = 0;
            volta--;
            pthread_cond_broadcast(&wait_thread);
            j++;
            pthread_mutex_unlock(&mutex_main);   
        }
    }

    printf("OK!!\n");

    /* Unindo threads */
    for(int i = 0; i < n; i++)
        if (pthread_join(tid[i], NULL)) {
            printf("\n Erro ao juntar a thread!");
            exit(1);
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
    int vel = 30;
    int pos_i = -1; // primeiro termo (0 a d-1) da posição na pista[d][10]
    int pos_j = -1; // segundo termo (0 a 9) da posição na pista[d][10]

    pthread_mutex_lock(&mutex);
    pos_j = insereNaPista(pthread_self()); 
    pos_i = ind_full;
    pthread_mutex_unlock(&mutex);

    while(volta != 0) {
        pthread_mutex_lock(&mutex);
        total++;
        printf("[%d][%d], %d eh total\n", pos_i, pos_j, total);
        pthread_cond_wait(&wait_thread, &mutex);
        
        int a = atualizaPos(pthread_self(), pos_i, pos_j);
        if(a)
            pos_i++;
        
        pthread_mutex_unlock(&mutex);
    }

    printf("a thread %ld saiu\n", pthread_self());

    /* INICIALMENTE: */
    // Os ciclistas largam em fila ordenados aleatoriamente com no máximo 5 ciclistas 
    // lado a lado em cada posição.
    // obs: todos ciclistas começam com velocidade inicial 30Km/h (1m a cada 120ms)!!
    // pensando em criar variável para checar se deu a volta ou não -> teria q colocar d global

    // para as diferentes velocidades, pensei em rodar sempre o laço a uma dada velocidade e
    // colocar barreiras a mais caso a velocidade de alguns seja menor que a de outros

    /* LAÇO (provavelmente): */
    /* pthread_wait? -> sinal de passar tempo*/
    /* checa se precisa/dá pra ultrapassar outros amigos -> faz isso se possível com mutex */

    /* quando a chegar na outra rodada: laço? enquanto não chegou na rodada n roda isso */
    // checaria aqui a vairável se deu a volta ou não
    /* sortear nova velocidade */

    return NULL;
}

int insereNaPista(pthread_t thread) {

    int pos = 0;
    // checo se as posições em pista[ind_full][j] estão disponíveis
    while(pos < 5 && pista[ind_full][pos] != 0)
        pos++;

    printf("%d é a pos\n", pos);
    
    if( pos == 5 ) {
        ind_full++;
        pista[ind_full][0] = thread;
        pos = 0;
    }
    else {
        pista[ind_full][pos] = thread;
    }
    
    return pos;
}


int atualizaPos(pthread_t thread, int pos_i, int pos_j) {
    // CONSIDERANDO TODOS COMO VELOCIDADE 30KM/H
    // mutex?
    if(pos_i < tam_pista && !pista[pos_i+1][pos_j]) {
        pista[pos_i+1][pos_j] = thread;
        pista[pos_i][pos_j] = 0;
        return 1;
    }
    // mutex?

    return 0;
}