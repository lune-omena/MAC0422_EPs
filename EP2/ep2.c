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
int total_ciclistas = -1;           /* número total de ciclistas (n) */
int num_ciclistas = -1;             /* número de ciclistas ATUAL */
int volta_total = -1;               /* número de voltas */
int rodada = 0;                     /* rodadas -> voltas dadas na pista -> precisamos padronizar os nomes...*/
int total = -1;                     /* variável para que a main espere todas threads */
int ind_full = 0;                   /* índice da pista que se encontra "cheio" */
int tam_pista = 0;                  /* é igual a d */
//int final = 0;
double tempo = 60000;               /* 1.000.000 = 1seg. Ideal: 60.000 = 60ms ; */
int acabou = 0;                     /* o programa roda até essa variável se tornar 1 */
pthread_t ** assoc;                  /* associação de id da thread com rodada */
pthread_t * toBeDestroyed;           /* guarda id de threads que poderiam ter que ser destruídas OPCAO 2 */
Node * toDestroy = NULL;
int num_toDestroy = 0;

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
    total_ciclistas = num_ciclistas = n;

    /* SIMULAÇÃO */
    srand(21132344);
    pthread_mutex_init(&mutex_main, NULL);
    pthread_mutex_init(&mutex, NULL);
    
    // número de voltas na simulação
    volta = 0;
    volta_total = 46; //OBS PRECISA SER NÚMERO PAR PARA NÃO DAR MERDA
    int voltas = volta_total;

    // criando matriz de associação de id da thread para respectiva rodada
    // em assoc[i][0] encontra-se a identificação da thread
    // em assoc[i][1] encontra-se a rodada da respectiva thread
    assoc = (pthread_t **) malloc (n*sizeof(pthread_t *));

    for(int i = 0; i < n; i++) {
        assoc[i] = (pthread_t *) malloc (2*sizeof(pthread_t));

        // as posições inicialmente serão zero
        for(int j = 0; j < 2; j++) {
            assoc[i][j] = 0;
        }
    }

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
    // também ocorre a associação do ciclista à rodada em que está
    for(int i = 0; i < n; i++)
        if (pthread_create(&tid[i], NULL, thread, NULL)) {
            printf("\n ERROR creating thread\n");
            exit(1);
        }
        else {
            assoc[i][0] = tid[i];
            assoc[i][1] = rodada+1;
        }

    for(int i = 0; i < n; i++)
        printf("a thread %03ld está na rodada %ld\n", assoc[i][0]%1000, assoc[i][1]);

    /* A cada duas voltas o ciclista que completar a última volta na última posição é eliminado.
       A prova termina quando sobrar apenas um ciclista, que é o campeão.
    */  //while(n > 1)

    //for(int j = 0; j < voltas; ) {
    while (!acabou /* && !ult_volta*/) {

        if(total == num_ciclistas) { // MUDAR PARA NUMERO ATUAL DE CICLISTAS
            pthread_mutex_lock(&mutex_main);
            usleep(tempo);
            printf("...\n");
            total = 0;

            /* OPCAO 2 
                if(toDestroy && rodada%2 == 0) { //exceção rodada 0
                    int choice = rand()%(num_toDestroy);
                    Node * node_aux = toDestroy;
                    pthread_t id_toDestroy = 0;
                    
                    while(choice) {
                        node_aux = node_aux->prox;
                        choice--;
                    }

                    id_toDestroy = aux->id;
                    pthread_cancel(&id_toDestroy);
                    
                    node_aux = toDestroy;
                    toDestroy = NULL;
                    Node * aux2 = NULL;

                    while(aux) { // liberando toDestroy
                        aux2 = aux;
                        aux = aux->prox;
                        aux2->prox = NULL;
                        free(aux2);
                    }

                    // zerar posições da pista também!!!!!!!!!!!!!!!!!!!
                    // diminuir número de ciclistas correntes, provavelmente
                    // atualizar ranking 

                }
                    
            */

            // pelo que entendi, o código abaixo serve para checar se chegou na n-ésima rodada
            // isso significa que já teriam ocorrido um múltiplo do tamanho da pista de rodadas
            if((volta-rodada*10) % tam_pista == 0)
            {
                rodada++;
                getchar();
                printf("*** RODADA %3d ***\n", rodada);
            }
            
            volta++;

            // AQUI VAI FICAR A FUNÇÃO QUE CHECA A POSIÇÃO DE CADA UMA DAS THREADS
            // pthread_t aux = classifica(pista, tid);
                
            pthread_cond_broadcast(&wait_thread);
            //j++;
            pthread_mutex_unlock(&mutex_main);   
        }

        if(volta >= voltas) // temporário
            acabou = 1;
    }
    //}

    // ANTIGO LAÇO DE ITERAÇÃO
    /*
    for(int j = 0; j < voltas; ) {
        // Simulação com 5 "voltas" (1 volta = 1 segundo)
        if(total == n) {
            pthread_mutex_lock(&mutex_main);
            usleep(tempo);
            printf("...\n");
            total = 0;

            // pelo que entendi, o código abaixo serve para checar se chegou na n-ésima rodada
            // isso significa que já teriam ocorrido um múltiplo do tamanho da pista de rodadas
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
    }*/
    // FIM DO ANTIGO LAÇO DE ITERAÇÃO


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

    //*rodada = 0;
    *rodada = 1;
    *vel_atual = KM30;

    pthread_mutex_lock(&mutex);
    pos_j = insereNaPista(pthread_self()); 
    pos_i = ind_full;
    pthread_mutex_unlock(&mutex);
    int CHECK = 0;

    while(volta != volta_total && CHECK != 2 /*&& !final*/) 
    // CHECK é a condição de eliminação, se for 2 foi pq foi atuaizada em atualizaPos
    {
        pthread_mutex_lock(&mutex);
        total++;
        printf("[%2d][%2d], %2d eh total - vel: %d - ", pos_i, pos_j, total, *vel_atual);
        printf("rodada: %d - thread: %3ld\n", *rodada, pthread_self()%1000);
        
        pthread_cond_wait(&wait_thread, &mutex);
        
        if(*vel_atual == KM30) { // esperam 2 voltas
            total++;
            pthread_cond_wait(&wait_thread, &mutex);
        }
        // se não, é 60km/h e roda normal

        CHECK = atualizaPos(pthread_self(), pos_i, pos_j, rodada, vel_atual);
        if(CHECK == 1) // houve mudança -> importante já que ocorreram aquelas coisas da issue
        {
            if (pos_i < (tam_pista - 1))
                pos_i++;
            else 
                pos_i = 0;
        }

        /* funções a serem implementadas */
        //registraPosicao(pthread_self(), /* parametros para registro: tempo, volta, id, rank*/);
        //verificaQuebra(pthread_self(), /* mesmos parametros para registrar posição */);
        
        pthread_mutex_unlock(&mutex);
    }

    if(CHECK == 2)
        printf("\na thread %ld foi eliminada da corrida... e ", pthread_self()%1000);
    
    num_ciclistas--;
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

    pthread_cancel(pthread_self());

    return NULL;
}

int insereNaPista(pthread_t thread) {

    int pos = 0;
    // checo se as posições em pista[ind_full][j] estão disponíveis
    while(pos < 5 && pista[ind_full][pos] != 0)
        pos++;

    printf("A thread %03ld incia na posição %d\n", thread%1000, pos);
    
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

    /* Caso a velocidade seja 30km/h, bicicleta anda 1m por chamada */
    /* Se estou na posição d-1 (final da pista), tenho que voltar ao inicio */
    /* Logo, todas as posições antes de d-1 eu faço uma atualização simples */

    /* Caso a velocidade seja 60km/h, bicicleta anda 2m por chamada*/
    /* Raciocínio acima se aplica. Temos uma condição para atualização simples: */

    // INÍCIO NOVO CÓDIGO - edição
    if(pos_i < (tam_pista - 1)) {   
        if(!pista[pos_i + 1][pos_j]) {
            // printf("Velocidadeede: %d - thread: %3ld\n", *vel_atual, pthread_self()%1000);
            pista[pos_i + 1][pos_j] = thread;
            pista[pos_i][pos_j] = 0;
            return 1;
        }
    }
    else // última posição
        if (!pista[0][pos_j]) {

            // INCLUIR PARA QUALQUER UMA DAS OPÇÕES

            int menor = assoc[0][1];
            int cont = 1;
            int atual = 0;
            
            for(int i = 1; i < total_ciclistas; i++) {
                printf("%ld é a rodada de %ld\n", assoc[i][1], assoc[i][0]);
                if(assoc[i][1] != 0) { // apenas checando associações não eliminadas
                    if(assoc[i][1] < menor) {
                        printf("entrou\n");
                        cont = 1; //OPCAO 1
                        menor = assoc[i][1];
                        atual = i;
                    }
                    else if(menor == assoc[i][1])
                        cont++; // OPCAO 1
                }
            }

            printf("%d eh o menor valor com cont %d\n", menor, cont);

           // OPCAO 1 - o PC tecnicamente sorteia aleatoriamente

            if(*rodada%2 == 0 && *rodada == menor && cont == 1) {

                printf("entrou AQUIII\n");
                // zera posição na pista
                pista[pos_i][pos_j] = 0;

                // zera no vetor de associações
                assoc[atual][1] = 0;

                return 2; // 2 == SINAL QUE TEM QUE SAIR 
            }    
            else { //roda tudo abaixo ( menos OPCAO 2)

                // Caso esteja terminando a volta, define nova velocidade para proxima rodada
                // retornando ciclista para marcação do início da pista
                pista[0][pos_j] = thread;
                pista[pos_i][pos_j] = 0;

                // registrando início de nova rodada pessoal
                *rodada = *rodada+1;
                atualizaRodada(thread, *rodada, total_ciclistas);

                // atualizando velocidade
                *vel_atual = atualizaVel(*vel_atual, *rodada);

                return 1;
            }
            // FIM OPCAO 1

            /* OPCAO 2 - eu sorteio essa porra
                // obs: tenho medo de dar segfault...
                // e talvez necessidade de mutex...
                if(*rodada == menor )
                    if(!toDestroy) {
                        toDestroy = (Node *) malloc(sizeof(Node));
                        toDestroy->id = thread;
                        toDestroy->prox = NULL;
                    }
                    else {
                        Node * aux = toDestroy;
                        Node * new = (Node *) malloc(sizeof(Node));
                        new->id = thread;
                        new->prox = NULL;

                        while(aux->prox)
                            aux = aux->prox;
                        
                        aux->prox = new;
                    }
                    toBeDestroyed é atualizado
                    a decisão de deletar vai ser feita no escalonador ...

                roda o código abaixo mesmo assim (no caso de n ser deletado)
                    
            */

            // Caso esteja terminando a volta, define nova velocidade para proxima rodada
            // retornando ciclista para marcação do início da pista
            /*
            pista[0][pos_j] = thread;
            pista[pos_i][pos_j] = 0;

            // registrando início de nova rodada pessoal
            *rodada = *rodada+1;
            atualizaRodada(thread, *rodada, num_ciclistas);

            // atualizando velocidade
            *vel_atual = atualizaVel(*vel_atual, *rodada);

            return 1;
            */
        }

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

int atualizaRodada(pthread_t thread, int rodada, int n) {
    int achou = 0;

    for(int i = 0; i < n && !achou; i++)
        if (assoc[i][0] == thread) {
            assoc[i][1] = rodada;
            achou = 1;
        }

    return achou;
}

