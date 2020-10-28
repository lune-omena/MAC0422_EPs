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
#define KM30 1
#define KM60 2
#define KM90 1

/* Variáveis globais */
pthread_t ** pista;                 /* representa a pista dos ciclistas, possui [d][10] casas */
pthread_mutex_t mutex_main;         /* mutex usado para o escalonador */
pthread_cond_t wait_thread;         /* barreira (cond) para threads */
pthread_mutex_t mutex;
int volta = -1;                     /* número de voltas */
int volta_total = -1;               /* número de voltas */
int rodada = 0;                     /* rodadas -> voltas dadas na pista -> precisamos padronizar os nomes...*/
int total = -1;                     /* variável para que a main espere todas threads */
int ind_full = 0;                   /* índice da pista que se encontra "cheio" */
int tam_pista = 0;                  /* é igual a d */
int final = 0;
double tempo = 60000;                /* 1.000.000 = 1seg. Ideal: 60.000 = 60ms ; */

int main(int argc, char * argv[]) 
{
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

    d = 10; /* excluir quando estiver pronto */
    tam_pista = d;/* excluir quando estiver pronto */
    /* SIMULAÇÃO */
    srand(21132344);
    pthread_mutex_init(&mutex_main, NULL);
    pthread_mutex_init(&mutex, NULL);
    
    // número de voltas na simulação
    volta = 0;
    volta_total = 20;
    int voltas = volta_total;

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

    for(int j = 0; j < voltas; )
    { // Simulação com 5 "voltas" (1 volta = 1 segundo)
        if(total == n)
        {
            pthread_mutex_lock(&mutex_main);
            usleep(tempo);
            printf("...\n");
            total = 0;

            if((volta-rodada*10) % tam_pista == 0)
            {
                rodada++;
                getchar();
                printf("*** RODADA %3d ***\n", rodada);
            }
            
            volta++;
                
            pthread_cond_broadcast(&wait_thread);
            j++;
            pthread_mutex_unlock(&mutex_main);   
        }
    }


    /* Unindo threads */
    for(int i = 0; i < n; i++)
        if (pthread_join(tid[i], NULL)) {
            printf("\n Erro ao juntar a thread!");
            exit(1);
        }

    printf("OK!!\n");
    /* Fim da execução */

    // liberando memória...
    for(int i = 0; i < d; i++)
        free(pista[i]);

    free(pista);

    return 0;
}

void * thread(void * a)
{
    /* a thread vai ser criada e vai rodar este código yay <3*/

    int pos_i = -1; // primeiro termo (0 a d-1) da posição na pista[d][10]
    int pos_j = -1; // segundo termo (0 a 9) da posição na pista[d][10]
    int *rodada, *vel_atual;
    
    rodada = (int*) malloc(sizeof(int));
    vel_atual = (int*) malloc(sizeof(int));

    *rodada = 0;
    *vel_atual = KM30;

    pthread_mutex_lock(&mutex);
    pos_j = insereNaPista(pthread_self()); 
    pos_i = ind_full;
    pthread_mutex_unlock(&mutex);

    while(volta != volta_total && !final)
    {
        pthread_mutex_lock(&mutex);
        total++;
        printf("[%2d][%2d], %2d eh total - vel: %d - ", pos_i, pos_j, total, *vel_atual);
        printf("rodada: %d - thread: %3ld\n", *rodada, pthread_self()%1000);
        
        pthread_cond_wait(&wait_thread, &mutex);
        
        int a = atualizaPos(pthread_self(), pos_i, pos_j, rodada, vel_atual);
        if(a)
        {
            if (pos_i < (tam_pista - 1))
                pos_i += *vel_atual;
            else 
                pos_i = 0;
        }

        /* funções a serem implementadas */
        //registraPosicao(pthread_self(), /* parametros para registro: tempo, volta, id, rank*/);
        //verificaQuebra(pthread_self(), /* mesmos parametros para registrar posição */);
            
        
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


int atualizaPos(pthread_t thread, int pos_i, int pos_j, int *rodada, int *vel_atual) {
    // CONSIDERANDO TODOS COMO VELOCIDADE 30KM/H
    // mutex?

    /* Caso a velocidade seja 30km/h, bicicleta anda 1m por chamada*/
    /* Se estou na posição d-1 (final da pista), tenho que voltar ao inicio */
    /* Logo, todas as posições antes de d-1 eu faço uma atualização simples */

    /* Caso a velocidade seja 60km/h, bicicleta anda 2m por chamada*/
    /* Raciocínio acima se aplica. Temos uma condição para atualização simples: */
    if(pos_i < (tam_pista - *vel_atual))
    {   
        if(!pista[pos_i + *vel_atual][pos_j])
        {
            /* printf("Velocidadeede: %d - thread: %3ld\n", *vel_atual, pthread_self()%1000); */
            pista[pos_i + *vel_atual][pos_j] = thread;
            pista[pos_i][pos_j] = 0;
            return 1;
        }

    }
    else 
    if (!pista[0][pos_j])
    {
        /* Caso esteja terminando a volta, define nova velocidade para proxima rodada */
        /* retornando ciclista para marcação do início da pista */
        pista[0][pos_j] = thread;
        pista[pos_i][pos_j] = 0;

        /* registrando início de nova rodada pessoal */
        *rodada = *rodada+1;

        /* atualizando velocidade*/
        *vel_atual = atualizaVel(*vel_atual, *rodada);
        return 1;
    }
    /* A ideia é que se algum ciclista andar a 90km/h, será o final da corrida 
       e a variável tempo poderá ser mudada sem nenhum problema > o incremento 
       na velocidade mudará, e a condição de ultrapassagem tbm */

    // mutex?

    return 0;
}

int atualizaVel(int vel_ant, int volta)
{
    /* pensar em como identificar que estamos nas duas ultimas voltas */
    /* -> quando possuímos somente 2 ciclistas */
    /* 1 = 60ms = 60Km/h */
    /* 2 = 120ms = 30Km/h */
    /* 1 = 40ms = 90Km/k - Caso seja sorteado, tempo do programa será reduzido em 2*/
    int number;
    volta = 0; /* retirar qnd colocar condição das 2 ultimas voltas */

    number = rand() % 10;

    if (volta) /* 2 ultimas voltas */             
        if (number < 1)     /* 10% de chance de ser 90Km/h */ 
        {
            tempo = 20000;
            /* precisa atualizar a velocidade do outro ciclista amigo */
            return KM90;
        }

    if (vel_ant == KM30)    /* 80% de chance de ser 60km/h*/
        return (number < 8) ? KM60: KM30; 
    else 
    if (vel_ant == KM60)    /* 60% de chance de ser 60km/h*/
        return (number < 6) ? KM60: KM30;
    

    /* Só entrará nesse caso caso tenha sido sorteado como 90km/h
       nas 2 ultimas voltas, e essa será a ultima volta */
    return KM90;
}