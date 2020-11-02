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
int voltas_max = -1;                /* numero maximo de voltas da corrida */
int rodada = 0;                     /* rodadas: Voltas dadas na pista*/

int total_ciclistas = -1;           /* número total de ciclistas, é igual a (n) */
int num_ciclistas = -1;             /* número de ciclistas em cada rodada corrente*/
int ciclistas_atuais = -1;          /* variável para que a main espere todas threads */

int ind_full = 0;                   /* índice da pista que se encontra "cheio" */
int tam_pista = 0;                  /* é igual a d */
double tempo = 60000;               /* 1.000.000 = 1seg. Ideal: 60.000 = 60ms ; */
int acabou = 0;                     /* o programa roda até essa variável se tornar 1 */
int total_quebrados = 0;

pthread_t ** assoc;                 /* associação de id da thread com rodada */
Ranking * classThreads = NULL;      /* guarda as classificações das threads para cada rodada */
Node * toDestroy;                   /* guarda as threads que quebraram para serem eliminadas pelo escalonador*/

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
    voltas_max = 2*(n-1);
    
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

    // CLASSTHREADS: inicia a lista de classificação de threads
    classThreads = (Ranking *) malloc(sizeof(Ranking));
    classThreads->t_ranks = (pthread_t *) malloc(n*sizeof(pthread_t));
    classThreads->rodada = 1;
    classThreads->quebrados = total_quebrados;
    classThreads->ideal_ciclistas = total_ciclistas;
    classThreads->prox = NULL;

    for(int i = 0; i < n; i++) // nenhuma thread associada ao ranking, ainda
        classThreads->t_ranks[i] = 0;

    // TODESTROY: inicia a lista de quebrados
    toDestroy = NULL;

    // ASSOC: criando matriz de associação de id da thread para respectiva rodada
    // em assoc[i][0] encontra-se a identificação da thread
    // em assoc[i][1] encontra-se a rodada da respectiva thread
    // em assoc[i][2] encontra-se o status da respectiva thread
    assoc = (pthread_t **) malloc (n*sizeof(pthread_t *));

    for(int i = 0; i < n; i++)
    {
        assoc[i] = (pthread_t *) malloc (3*sizeof(pthread_t));

        // as posições inicialmente serão zero
        for(int j = 0; j < 3; j++) 
            assoc[i][j] = 0; // ACTIVE  == 0
    }

    // todas threads precisam rodar
    ciclistas_atuais = 0;

    /* A pista deve possuir 10 posições para cada d, mas no início da simulação apenas 5 estarão ocupadas */
    // lembrando que pthread_t = unsigned long int, estou zerando todas posições antes de ocupá-las
    pista = (pthread_t **) malloc(d*sizeof(pthread_t *)); //[d][10]

    for(int i = 0; i < d; i++)
    {
        pista[i] = (pthread_t *) malloc(10*sizeof(pthread_t));

        for(int j = 0; j < 10; j++) 
            pista[i][j] = 0;
    }

    /* Seu simulador deve criar n threads “ciclista” iguais */
    // portanto, devo alocar n mutex (para cada thread)
    pthread_t tid[n];

    // Assim que houver a "largada", os ciclistas serão criados:
    // também ocorre a associação do ciclista à rodada em que está
    // mas como todos os ciclistas já estão ativos, não há necessidade de mudar
    for(int i = 0; i < n; i++) {
        pthread_mutex_lock(&mutex_main);

        int * aux = malloc(sizeof(int));
        *aux = i;
        
        //if (pthread_create(&tid[i], NULL, thread, NULL))
        if (pthread_create(&tid[i], NULL, thread, aux))
        {
            printf("\n ERROR creating thread\n");
            exit(1);
        }

        pthread_mutex_unlock(&mutex_main);

    }

    //for(int i = 0; i < n; i++)
    //    printf("a thread %03ld está na rodada %ld\n", assoc[i][0]%1000, assoc[i][1]);

    /* A cada duas voltas o ciclista que completar a última volta na última posição é eliminado.
       A prova termina quando sobrar apenas um ciclista, que é o campeão. */
    /* Começo da corrida*/
    while (num_ciclistas >= 1) 
        if(ciclistas_atuais == num_ciclistas)  // ciclistas atuais
        { 
            pthread_mutex_lock(&mutex_main);
            usleep(tempo);
            printf("...\n");
            ciclistas_atuais = 0;     
            volta++;

            quebrou(toDestroy);
                
            pthread_cond_broadcast(&wait_thread);
            pthread_mutex_unlock(&mutex_main);   
        }

   // while(num_ciclistas == 1) // isso é meio gambiarra mas wtv // Lucy: Acho que não precisamos mais!
   // { 
   //     acabou = 1;             // sinaliza para a última thread finalizar 
   //     pthread_cond_broadcast(&wait_thread);
   // }

    printf("OK!!\n");

    /* Unindo threads */
    for(int i = 0; i < n; i++)
        if (pthread_join(tid[i], NULL)) {
            printf("\n Erro ao juntar a thread!");
            exit(1);
        }

    /* Fim da execução */

    // liberando memória
    for(int i = 0; i < d; i++)
        free(pista[i]);

    free(pista);

    return 0;
}

void * thread(void * a)
{
    /* a thread vai ser criada e vai rodar este código yay <3*/

    int pos_i = -1;               // primeiro termo (0 a d-1) da posição na pista[d][10]
    int *pos_j;                   // segundo termo (0 a 9) da posição na pista[d][10]
    int *rodada, *vel_atual;
    int CHECK = 0;
    int * i = (int *) a;
    assoc[*i][0] = pthread_self();
    assoc[*i][1] = 1;

    //printf("Thread %ld associada ao índice %d\n", assoc[*i][0]%1000, *i);

    rodada = (int*) malloc(sizeof(int));
    vel_atual = (int*) malloc(sizeof(int));
    pos_j = (int*) malloc(sizeof(int));
    *rodada = 1;
    *vel_atual = KM30;
    int delete = 0;

    pthread_mutex_lock(&mutex);
    *pos_j = insereNaPista(pthread_self()); 
    pos_i = ind_full;
    pthread_mutex_unlock(&mutex);

    /* Enquanto thread não foi marcada para ser eliminada ou 
       não terminou a quantidade máxima de volta possível na corrida 
       > CHECK é a condição de eliminação, se for 2 foi pq foi atuaizada em atualizaPos */
    while(CHECK != TOBEDELETED && CHECK != LATEDELETION &&  !delete && *rodada <= voltas_max) 
    {
        pthread_mutex_lock(&mutex);
        ciclistas_atuais++;
        printf("[%3d][%2d], %2d eh total - vel: %d - ", pos_i, *pos_j, ciclistas_atuais, *vel_atual);
        printf("rodada: %d - thread: %3ld\n", *rodada, pthread_self()%1000);
    
        pthread_cond_wait(&wait_thread, &mutex);

        if(*i != -1 && (assoc[*i][2] == TOBEDELETED || acabou || assoc[*i][2] == BROKEN))
        {
            delete = 1;
        }

        if(!delete)
        {
            if(*vel_atual == KM30)
            { // esperam 2 voltas
                ciclistas_atuais++;
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

        }
        /* funções a serem implementadas */
        //registraPosicao(pthread_self(), /* parametros para registro: tempo, volta, id, rank*/);
        //verificaQuebra(pthread_self(), /* mesmos parametros para registrar posição */);
        
        pthread_mutex_unlock(&mutex);
    }

    // OPCAO 1 E 2
    if(*rodada > voltas_max)
        printf("\nA thread %ld completou a corrida e espera pelos seus concorrentes.\n", pthread_self()%1000);
    else if(assoc[*i][2] == BROKEN)
        printf("A thread %ld quebrou na rodada %d!\n", pthread_self()%1000, *rodada+1);
    else if(CHECK == LATEDELETION)
        printf("A thread %ld foi eliminada da corrida de forma atrasada\n", pthread_self()%1000);
    else if(CHECK == 2 || delete)
        printf("\na thread %ld foi eliminada da corrida... e \n", pthread_self()%1000);
    
    //assoc[*i][1] = 0;
   
    num_ciclistas--;
    //printf("a thread %ld saiu\n", pthread_self());

    assoc[*i][2] = DELETED;

    //printf("%d eh o número de ciclistas agora\n", num_ciclistas);

    pthread_cancel(pthread_self());

    return NULL;
}

int insereNaPista(pthread_t thread)
{
    int pos = rand() % 2;
    int posicoes_ocupadas = 0;   
    
    for (int i = 0; i < 10; i++)
    {
        if (pista[ind_full][i] != 0)
            posicoes_ocupadas++;
    }
    //printf("\nposicoes ocupadas: %d\n", posicoes_ocupadas);
    if (posicoes_ocupadas == 5) 
        ind_full++;

    while(pos < 10 && pista[ind_full][pos] != 0)
    {
        pos = rand() % 10;
       // printf("Posicao sorteada: %d - thread: %ld\n", pos, pthread_self() % 1000);
    }
        
   // printf("A thread %03ld incia na posição %d\n", thread%1000, pos);
    pista[ind_full][pos] = thread;
    
    return pos;
}


int atualizaPos(pthread_t thread, int pos_i, int *pos_j, int *rodada, int *vel_atual) {

    if(pos_i < (tam_pista - 1)) {   
        if(!pista[pos_i + 1][*pos_j]) {
            // printf("Velocidadeede: %d - thread: %3ld\n", *vel_atual, pthread_self()%1000);
            pista[pos_i + 1][*pos_j] = thread;
            pista[pos_i][*pos_j] = 0;
            return 1;
        }
        else /* condições de ultrapassagem */
        {
            if (*pos_j + 1 < 10)
            {
                if(!pista[pos_i][*pos_j + 1] && !pista[pos_i + 1][*pos_j + 1])
                {
                    printf("\n\nCONDIÇÃO DE ULTRAPASSAGEM UTILIZADA - THREAD: %ld\n",  pthread_self()%1000);
                    pista[pos_i + 1][*pos_j + 1] = thread;
                    pista[pos_i][*pos_j] = 0;
                    *pos_j = *pos_j + 1;
                    return 1;
                }
            }
            else
            if (*pos_j - 1 >= 0)
            {
                if (!pista[pos_i][*pos_j - 1] && !pista[pos_i + 1][*pos_j - 1])
                {
                    printf("\nCONDIÇÃO DE ULTRAPASSAGEM UTILIZADA - THREAD: %ld\n\n",  pthread_self()%1000);
                    pista[pos_i + 1][*pos_j - 1] = thread;
                    pista[pos_i][*pos_j] = 0;
                    *pos_j = *pos_j - 1;
                    return 1;
                }
            }
        }
        
    }
    else // última posição
        if (!pista[0][*pos_j])
        {
            // POSSIBILIDADE DE QUEBRA...
            if((*rodada+1)%6 == 0 && num_ciclistas > 5) { // rodada multipla de 6 deve possibilitar quebra de ciclista
                int r_num = rand()%100;

                if(r_num < 5) { // o ciclista irá quebrar! :(
                    // adiciona na lista de threads a serem quebradas
                    // precisa saber em qual rodada está e quantos ciclistas poderão passar pra próxima rodada

                    Node * new = (Node *)malloc(sizeof(Node));
                    new->id = pthread_self();
                    new->prox = NULL;
                    new->rodada_pessoal = *rodada+1;

                    if(!toDestroy)
                        toDestroy = new;
                    else {
                        Node * aux = toDestroy;
                        
                        while(aux->prox)
                            aux = aux->prox;
                        
                        aux->prox = new;
                    }

                }

            }
            // FIM QUEBRA

            if (atualiza_Classificacao(thread, rodada, 1) == TOBEDELETED)
            {
                pista[pos_i][*pos_j] = 0;
                assoc[findThread(thread)][2] = TOBEDELETED;
                return TOBEDELETED; // return 2
            }
            
            if(*rodada == maior())
            {                   
                printf("*** RODADA %3d ***\n", *rodada);
                getchar();
            }

            // Caso esteja terminando a volta, define nova velocidade para proxima rodada
            // retornando ciclista para marcação do início da pista
            pista[0][*pos_j] = thread;
            pista[pos_i][*pos_j] = 0;

            // registrando início de nova rodada pessoal
            *rodada = *rodada+1;
            atualizaRodada(thread, *rodada, total_ciclistas);

            // atualizando velocidade
            *vel_atual = atualizaVel(*vel_atual, *rodada);

            return 1;

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

int findThread(pthread_t thread) {
    int value = -1;

    for(value = 0; value < total_ciclistas && assoc[value][0] != thread; value++);

    return value;  
}

int maior() { // devolve o número da rodada do primeiro colocado
    int maior = 0;

    for(int i = 0; i < total_ciclistas; i++)
        if((assoc[i][2] == ACTIVE) && (assoc[i][1] != 0 && maior < assoc[i][1]))
            maior = assoc[i][1];

    return maior;
}

int atualiza_Classificacao(pthread_t thread, int * rodada, int verbose)
{
    Ranking * rank_aux = classThreads;
    int i, j;

    while(rank_aux != NULL && rank_aux->rodada != *rodada)
        rank_aux = rank_aux->prox;
    
    if (rank_aux == NULL)
        return DELETED;
    
    if(assoc[findThread(thread)][2] == LATEDELETION) {
        printf("Oi! eu, thread %ld, devia ter sido eliminada antes (cheguei em último)...\n", thread);
        // possivelmente imprimir qual rodada que chegou por último.
        return LATEDELETION;
    }
    // Caso seja primeiro ciclista a iniciar a rodada
    else if (*rodada == maior() && !rank_aux->t_ranks[0])
    {
        printf("%ld EH O PRIMEIRO COLOCADO!!!!!!!\n", thread%1000);

        rank_aux->t_ranks[0] = thread;

        /* Reservando espaço para a próxima rodada */
        Ranking * rank_new = (Ranking *) malloc(sizeof(Ranking));
        rank_new->prox = NULL;
        rank_new->quebrados = total_quebrados;
        rank_new->ideal_ciclistas = total_ciclistas - ((int) (*rodada + 1)/2);
        rank_new->rodada = *rodada + 1;
        rank_new->t_ranks = (pthread_t *) malloc(rank_new->ideal_ciclistas * sizeof(pthread_t));

        // zerando todas as posições fora a primeira
        for(j = 0; j < rank_new->ideal_ciclistas; j++)
            rank_new->t_ranks[j] = 0;

        rank_aux->prox = rank_new;

        //printf("Ideal ciclistas: %d\n", rank_aux->ideal_ciclistas);
        /* Caso seja a única posição disponível - apagar depois de pronto */
        if (rank_aux->ideal_ciclistas - rank_aux->quebrados == 1)
        {
            // mostrarRanking com vencedor;
            printf("%ld EH O VENCEDOR!!!!!!!\n", thread%1000);
        }
    }
    else
    {
        /* caso não seja primeiro ciclista a iniciar a rodada */

        /* Encontrando posição livre em t_ranks 
           começa do 1 porque no 0 se encontra o primeiro a passar na rodada */
        for(i = 1;(i < total_ciclistas + 1 - *rodada/2 - rank_aux->quebrados)
               && (rank_aux->t_ranks[i] != 0); i++);

        /* Inserindo na posição correta do vetor t_ranks 
           Se for último elemento de uma rodada par -> será eliminado */
        if(i == total_ciclistas - *rodada/2 - rank_aux->quebrados && *rodada%2 == 0)
        { 
            /* Liberando lista de rankings anteriores */
            rank_aux = classThreads;
            classThreads = classThreads->prox;

            free(rank_aux->t_ranks);
            free(rank_aux);
 
            printf("Ciclista %ld será deletado...\n", thread%1000);

            return TOBEDELETED; 
        }
        else
        {
            rank_aux->t_ranks[i] = thread;
        }            
    }
    
    /* Caso tenha completado a lista de participantes da rodada,
       exibe classificações */
    if (rank_aux->t_ranks[rank_aux->ideal_ciclistas - 1] && verbose)
    {
        printf("\nRanking %d :", *rodada);
        
        for(int j = 0; j < rank_aux->ideal_ciclistas; j++) 
            printf("%ld ", rank_aux->t_ranks[j]%1000);
    
        printf("\n");
    }
    
    return ACTIVE;
}

int quebrou(Node * toDestroy) {

    // PRECISO CONSIDERAR QUANTOS QUEBRARAM ANTES DESSE TBM!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1
    // ZERAR QUEBRADOS NO MOMENTO QUE FOR ALOCADO O PRIMEIRO
    // RECEBE O VALOR DE QUEBRADOS DO ANTERIOR
    // AQUI ATUALIZA NÚMERO DE QUEBRADOS PROS PŔOXIMOS

    if(toDestroy) { // as threads a serem destruídas entram aqui
        Node * d_aux = toDestroy;
        Ranking * r_aux = classThreads;

        int ** quebra_rodada = (int **) malloc(total_ciclistas*sizeof(int));

        for(int i = 0; i < total_ciclistas; i++) {
            quebra_rodada[i] = (int *) malloc(2*sizeof(int));
            quebra_rodada[i][0] = 0;
        }

        int j = 0;

        while(d_aux) {
            // ATUALIZO O NÚMERO DE QUEBRADOS DE CADA RODADA
            // lembrando que as threads que quebraram "estão" numa rodada anterior
            int i;
            for(i = 0; i < total_ciclistas && quebra_rodada[i][0] != 0 &&
            quebra_rodada[i][0] != d_aux->rodada_pessoal; i++);

            if(quebra_rodada[i][0] ==  d_aux->rodada_pessoal) {
                quebra_rodada[i][1]++;
            }
            else if(quebra_rodada[i][0] == 0) {
                quebra_rodada[i][0] = d_aux->rodada_pessoal;
                quebra_rodada[i][1] = 1;
            }

            // ATUALIZA STATUS DA THREAD
            assoc[findThread(d_aux->id)][2] = BROKEN;

            j++;
            d_aux->prox;
        }

        if(j < total_ciclistas) // -1 para último elemento sinaliza fim da lista de rodadas
            quebra_rodada[j][0] = quebra_rodada[j][1] = -1;

        // INSIRO EM CLASSTHREADS PARA CADA RODADA IDENTIFICADA O NÚMERO DE QUEBRADOS
        j = 0;

        while(r_aux && j < total_ciclistas && quebra_rodada[j][0] != -1) {

            if(r_aux->rodada == quebra_rodada[j][0]) {
                r_aux->quebrados = quebra_rodada[j][1];

                // CHECO SE EXISTE THREAD QUE JÁ PASSOU QUE SERIA A ÚLTIMA (TOBEDELETED)
                int i = total_ciclistas - r_aux->rodada/2 - r_aux->quebrados;

                if(i > -1 && r_aux->t_ranks[i] != 0) { // vou ter que mudar isso...
                    assoc[findThread(r_aux->t_ranks[i])][2] = LATEDELETION;
                }
                else if(i < 0){
                    printf("Algo deu errado. Valor inválido da soma!\n");
                }

                j++;
            }

            // TRATAR COMO DELETAR ESSAS QUE QUEBRARAM
            // TRATAR O VETOR DE CÉLULAS QUE CONTÉM OS NÚMEROS VAGOS COMO 0!

            r_aux = r_aux->prox;
        }


        for(int i = 0; i < total_ciclistas; i++)
            free(quebra_rodada[i]);
        
        free(quebra_rodada); 

        return 1;

    }

    return 0;

}