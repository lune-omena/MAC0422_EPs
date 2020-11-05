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
#include <time.h>
#define KM30 1
#define KM60 2
#define KM90 3

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
int total_quebrados = 0;

int t_ultimos = 0;                  /* identifica se são os últimos a passar para permitir rodar */
int segundo_acabou = 0;             /* usado para checar se há necessidade de manter possibilidade de 90km/h */
int vel_ultimos[2];                 /* guarda velocidade dos 2 finalistas*/
pthread_t finalistas[2];            /* guarda id dos 2 finalistas*/

pthread_t ** assoc;                 /* associação de id da thread com rodada */
Ranking * classThreads = NULL;      /* guarda as classificações das threads para cada rodada */
RankingGeral * general = NULL;           /* Guarda informações de quando uma thread sai da rodada */
Node * toDestroy;                   /* guarda as threads que quebraram para serem eliminadas pelo escalonador*/

struct timespec t_start;            /* Armazenará tempo de inicio do programa */
struct timespec t_end;              /* Armazenará tempo de finalização do programa */
int debug = 0;                          /* Indica modo de debug */

int main(int argc, char * argv[]) 
{
    printf("EP2 - Ciclistas!\n");

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
    total_ciclistas = num_ciclistas = n;

    if(d < 250) {
        printf("Insira um comprimento de velódromo (d) maior ou igual a 250.\n");
        exit(EXIT_FAILURE);
    }
    else if(n < 5 || n > 5*d ) {
        printf("Insira um número de ciclistas (n) maior que 5 e menor ou igual a (d*5).\n");
        exit(EXIT_FAILURE);
    }
    if (argc == 4 && *argv[3] == 'd') {
        debug = 1;
        printf("\n Modo debug ativado!\n");
    }

    //d = 15; /* excluir quando estiver pronto */
    tam_pista = d;/* excluir quando estiver pronto */
    
    // todas threads precisam rodar
    ciclistas_atuais = 0;

    /* SIMULAÇÃO */
    srand( clock() );

    /* Registrando tempo de início do programa */
    clock_gettime(CLOCK_MONOTONIC, &t_start);

    /* Inicializa rankings do programa */
    inicializa_Rankings();

    /* ASSOC: criando matriz de associação de id da thread para respectiva rodada */
    inicializa_assoc_pista();

    pthread_mutex_init(&mutex_main, NULL);
    pthread_mutex_init(&mutex, NULL);

    /* TODESTROY: inicia a lista de quebrados */
    toDestroy = NULL;

    /* Criando n threads “ciclista” iguais */
    pthread_t tid[n];

    // inicializa como se finalistas fossem de id 0
    finalistas[0] = finalistas[1] = 0;

    // inicializando velocidade dos ultimos como 0
    vel_ultimos[0] = vel_ultimos[1] = 0;

    // Assim que houver a "largada", os ciclistas serão criados:
    // também ocorre a associação do ciclista à rodada em que está
    // mas como todos os ciclistas já estão ativos, não há necessidade de mudar
    for(int i = 0; i < total_ciclistas; i++)
    {
        pthread_mutex_lock(&mutex_main);

        int * aux = malloc(sizeof(int));
        *aux = i;
        
        if (pthread_create(&tid[i], NULL, thread, aux))
        {
            printf("\n ERROR creating thread\n");
            exit(1);
        }

        pthread_mutex_unlock(&mutex_main);
    }
    
    // debug
    //for(int i = 0; i < n; i++)
    //    printf("a thread %03ld está na rodada %ld\n", assoc[i][0]%1000, assoc[i][1]);
    
    /* Começo da corrida */
    while (num_ciclistas >= 1) 
        if(ciclistas_atuais == num_ciclistas)  // ciclistas atuais
        { 
            pthread_mutex_lock(&mutex_main);
            usleep(tempo);

            if (debug)
                mostra_Pista();
                
           // printf("...\n");
            ciclistas_atuais = 0;     
            volta++;

            if(toDestroy) {
                quebrou();

                if(!toDestroy)
                    printf("LISTA DELETADA 2\n");
                else {
                    printf("Por algum motivo isso não funcionou...\n");
                    Node * aux = toDestroy;

                    printf("Ainda possui: ");

                    while(aux)
                    {
                        printf("%ld (%d) ", aux->id%1000, aux->rodada_pessoal);
                        aux = aux->prox;
                    }

                    printf("\n");
                    toDestroy = NULL;
                }
            }
                
            pthread_cond_broadcast(&wait_thread);
            pthread_mutex_unlock(&mutex_main);   
        }



    /* Unindo threads */
    for(int i = 0; i < n; i++)
        if (pthread_join(tid[i], NULL))
        {
            printf("\n Erro ao juntar a thread!");
            exit(1);
        }

    printf("OK!!\n");

    /* Fim da execução */
    mostra_Ranking();

    /* Liberando memória */
    free_program();

    clock_gettime(CLOCK_MONOTONIC, &t_end);
    printf("\n\nTempo de execução: %f\n", (double) (t_end.tv_sec - t_start.tv_sec) +
               (double) (t_end.tv_nsec - t_start.tv_nsec) / 1000000000.0);

    return 0;
}

void * thread(void * a)
{
    /* a thread vai ser criada e vai rodar este código yay <3*/

    int pos_i = -1;               // primeiro termo (0 a d-1) da posição na pista[d][10]
    int *pos_j;                   // segundo termo (0 a 9) da posição na pista[d][10]
    int *rodada, *vel_atual;
    int * id = (int *) a;
    int CHECK = 0;
    int delete = 0;

    rodada = (int*) malloc(sizeof(int));
    vel_atual = (int*) malloc(sizeof(int));
    pos_j = (int*) malloc(sizeof(int));

    *rodada = 1;
    *vel_atual = KM30;

    assoc[*id][0] = pthread_self();
    assoc[*id][1] = 1;

    pthread_mutex_lock(&mutex);
    *pos_j = insereNaPista(pthread_self()); 
    pos_i = ind_full;
    pthread_mutex_unlock(&mutex);

    /* Enquanto thread não foi marcada para ser eliminada ou 
       não terminou a quantidade máxima de volta possível na corrida 
       > CHECK é a condição de eliminação, se for 2 foi pq foi atuaizada em atualizaPos */
    while(CHECK != TOBEDELETED && CHECK != LATEDELETION && !delete && CHECK != CHAMPION
     /*&& *rodada <= voltas_max*/) 
    {
        pthread_mutex_lock(&mutex);
        ciclistas_atuais++;
        // debug
        //printf("[%3d][%2d], %2d eh total - vel: %d - ", pos_i, *pos_j, ciclistas_atuais, *vel_atual);
        //printf("rodada: %d - thread: %04d\n", *rodada, *id);
    
        pthread_cond_wait(&wait_thread, &mutex);

        if(assoc[*id][2] == BROKEN) {
            pista[pos_i][*pos_j] = 0;
        }

        if(*id != -1 && (assoc[*id][2] == TOBEDELETED || assoc[*id][2] == BROKEN))
        {
            delete = 1;

            // a parte de classificação de caras que quebraram seria melhor se estivesse aqui
        }

        if(!delete)
        {
            // se for t_ultimos são 2 para 90, 3 para 60, 6 para 30
            
            if(t_ultimos) {
                int itr = -1;

                switch(*vel_atual) {
                    case KM30:
                        itr = 5;  // 5 + 1 = 6
                        break;
                    case KM60:
                        itr = 2;  // 2 + 1 = 3
                        break;
                    case KM90:
                        itr = 1;  // 1 + 1 = 2
                        break;
                    default:
                        printf("Algo deu errado.\n");
                        exit(EXIT_FAILURE);
                };

                for(int i = 0; i < itr; i++) { 
                    ciclistas_atuais++;
                    pthread_cond_wait(&wait_thread, &mutex);

                    if(!t_ultimos) // pode ocorrer de não estar mais a 20ms
                        break;
                }

            }
            else if(*vel_atual == KM30)
            { // esperam 2 voltas
                ciclistas_atuais++;
                pthread_cond_wait(&wait_thread, &mutex);

            }   
            
            // se não, é 60km/h e roda normal
            CHECK = atualizaPos(pthread_self(), pos_i, pos_j, rodada, vel_atual, id);
            if(CHECK == 1) // houve mudança -> importante já que ocorreram aquelas coisas da issue
            {
                if (pos_i < (tam_pista - 1))
                    pos_i++;
                else 
                    pos_i = 0;
            }

        }
        
        pthread_mutex_unlock(&mutex);
    }

    // OPCAO 1 E 2
    if(CHECK == CHAMPION)
        printf("O ciclista %04d é o VENCEDOR da corrida na rodada %d!\n", *id, *rodada);

    if(*rodada > voltas_max)
        printf("\nO ciclista %04d completou a corrida e espera pelos seus concorrentes.\n", *id);
    else if(assoc[*id][2] == BROKEN)
        printf("O ciclista %04d quebrou na rodada %d!\n", *id, *rodada);
    else if(assoc[*id][2] == LATEDELETION)
        printf("O ciclista %04d foi eliminado da corrida de forma atrasada\n", *id);
    else if(CHECK == 2 || delete)
        printf("\nO ciclista %04d foi eliminado da corrida...\n", *id);


    assoc[*id][2] = DELETED;

    num_ciclistas--;

    //debug
    //printf("a thread %ld saiu\n", pthread_self());
    //printf("%d eh o número de ciclistas agora\n", num_ciclistas);

    free(rodada);
    free(vel_atual);
    free(pos_j);
    free(id);
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


int atualizaPos(pthread_t thread, int pos_i, int *pos_j, int *rodada, int *vel_atual, int * id)
{
    int a, b;

    if(pos_i < (tam_pista - 1)) {   
        if(!pista[pos_i + 1][*pos_j])
        {   /* caso padrão - há espaço na frente */
            // printf("Velocidadeede: %d - thread: %3ld\n", *vel_atual, pthread_self()%1000);
            pista[pos_i + 1][*pos_j] = thread;
            pista[pos_i][*pos_j] = 0;
            return 1;
        }
        else 
        {   /* condições de ultrapassagem */
            
            for (a = *pos_j + 1; a < 10; a++)
            {   //verifica espaço 'descendo'
                // encontrando espaço para ultrapassagem
                if (!pista[pos_i][a] && !pista[pos_i+1][a])
                {
                    for (b = a; b > (*pos_j + 1) && !pista[pos_i + 1][b]; b--);
                    
                    // debug
                    //printf("\t\tULTRAPASSAGEM DESCENDO USADA -  CICLISTA %04d\n", *id);
                    pista[pos_i + 1][b] = thread;
                    pista[pos_i][*pos_j] = 0;
                    *pos_j = b;
                    return 1;
                }
            }
            
            for (a = *pos_j - 1; a >= 0; a--)
            {   //verifica espaço 'subindo'
                // encontrando espaço para ultrapassagem
                if (!pista[pos_i][a] && !pista[pos_i + 1][a])
                {
                    for (b = a; b < (*pos_j - 1) && !pista[pos_i + 1][b]; b++);

                    //debug
                    //printf("\t\tULTRAPASSAGEM SUBINDO USADA -  CICLISTA %04d\n", *id);
                    pista[pos_i + 1][b] = thread;
                    pista[pos_i][*pos_j] = 0;
                    *pos_j = b;
                    return 1;
                }
            }
        }
        
    }
    else // última posição
        if (!pista[0][*pos_j])
        {

            // POSSIBILIDADE DE QUEBRA...
            if( assoc[*id][2] != LATEDELETION && (*rodada+1)%6 == 0) {

                Ranking * r = classThreads;
                int quebrados;

                // Considerando ciclistas quebrados
                while(r && r->rodada != *rodada)
                    r = r->prox;

                if(r && r->prox)  // existem rodadas a frente
                    quebrados = r->prox->quebrados;
                else
                    quebrados = r->quebrados;

                // Preciso considerar as threads que estão quebrando agora, também!
                if(toDestroy) {
                    //UGH
                    Node * d_aux = toDestroy;

                    while(d_aux) {
                        if(d_aux->rodada_pessoal == *rodada + 1)
                            quebrados++;
                        
                        d_aux = d_aux->prox;
                    }
                }

                // r->ideal_ciclistas - 1 - 0 == "filtro" original de quantos passam (a rodada sempre é par)
                // quebrados == quantos quebraram até agora e vão quebrar ainda, nessa iteração
                // -1 == ESTA THREAD LOL
                int min = r->ideal_ciclistas - 1 - quebrados - 1;
                        
                if(min > 5) { 

                    //printf("Dados sobre a rodada %d: %d (esperado), %d (quebrados)\n", *rodada+1, r->ideal_ciclistas, quebrados);

                    int r_num = rand()%100; // mudar para quantidade de ciclistas da rodada

                    if(r_num < 5) { // o ciclista irá quebrar! :(
                                    // adiciona na lista de threads a serem quebradas
                                    // precisa saber em qual rodada está e quantos ciclistas 
                                    // poderão passar pra próxima rodada

                        printf("\nA thread %04d iniciou o processo de quebra.\n", *id);

                        Node * new = (Node *)malloc(sizeof(Node));
                        new->id = pthread_self();
                        new->prox = NULL;
                        new->rodada_pessoal = *rodada+1;
                        
                        // ATUALIZA STATUS DA THREAD, NÃO DELETAR AINDA
                        assoc[*id][2] = BROKEN;

                        if(!toDestroy) {
                            printf("A LISTA ESTAVA VAZIA, PREENCHENDO ELA AGORA...\n");
                            toDestroy = new;
                        }
                        else {
                            Node * aux = toDestroy;
                            
                            while(aux->prox)
                                aux = aux->prox;
                            
                            aux->prox = new;
                        }

                    }

                }
            }
            
            // FIM QUEBRA

            int check = atualiza_Classificacao(thread, rodada, id, 1);
         
            if (check == TOBEDELETED)
            {
                pista[pos_i][*pos_j] = 0;
                assoc[*id][2] = TOBEDELETED;
                return TOBEDELETED; // return 2
            }
            else if(check == CHAMPION)
            {   // EITA CARALHA
                *vel_atual = KM60;

                // preciso atualizar para não ter chance de correr a 90km/h
                // se o segundo não estiver a 90km/h
                if(vel_ultimos[0] == pthread_self())
                    vel_ultimos[0] = KM60;
                else
                    vel_ultimos[1] = KM60;

                pista[pos_i][*pos_j] = 0;
                
                return CHAMPION;
            }
            
            if(*rodada == maior())
            {                   
                printf("*** RODADA %3d ***\n", *rodada);
                //getchar();
            }

            // Caso esteja terminando a volta, define nova velocidade para proxima rodada
            // retornando ciclista para marcação do início da pista
            pista[0][*pos_j] = thread;
            pista[pos_i][*pos_j] = 0;

            // registrando início de nova rodada pessoal
            *rodada = *rodada+1;
            atualizaRodada(thread, *rodada, total_ciclistas);

            // atualizando velocidade
            *vel_atual = atualizaVel(*vel_atual, *rodada, pthread_self());

            return 1;

        }

    return 0;
}

int atualizaVel(int vel_ant, int volta, pthread_t t)
{
    /* pensar em como identificar que estamos nas duas ultimas voltas */
    /* -> quando possuímos somente 2 ciclistas */
    /* 1 = 60ms = 60Km/h */
    /* 2 = 120ms = 30Km/h */
    /* 1 = 40ms = 90Km/k - Caso seja sorteado, tempo do programa será reduzido em 2*/
    int number = rand() % 10;

    if((finalistas[0] == t || finalistas[1] == t) && !segundo_acabou)
    {
        // recebe velocidade 90km/h se sorteado
        printf("*************************************************************\n");
        printf("entrou aqui: A THREAD %ld É UMA DAS 2 FINALISTAS (RODADA: %d)!\n", t%1000, volta);
        printf("*************************************************************\n");

        int me, you;

        if(finalistas[0] == t) {
            me = 0;
            you = 1;
        }
        else {
            me = 1;
            you = 0;
        }
         
        if (number < 5)     /* 10% de chance de ser 90Km/h */ 
        {
            t_ultimos = 1; // esta variável pode chegar aqui como 0, ela precisa ser atualizada sempre aqui
            printf("a thread %ld está a 90km/h\n", t%1000);
            vel_ultimos[me] = KM90;
            tempo = 20000;
            return KM90;
        }
        else if(vel_ultimos[you] != KM90)
        { /* checo se os dois não estão */
            tempo = 60000;
            t_ultimos = 0;

            printf("As duas threads estão a menos que 90km/h (possivelmente) \n");

            if (vel_ant == KM30) {    /* 80% de chance de ser 60km/h*/
                if(number < 8) {
                    vel_ultimos[me] = KM60;
                    return KM60;
                }
                else {
                    vel_ultimos[me] = KM30;
                    return KM30;
                }
            }
            else if (vel_ant == KM60) {   /* 60% de chance de ser 60km/h*/
                if(number < 6) {
                    vel_ultimos[me] = KM60;
                    return KM60;
                }
                else {
                    vel_ultimos[me] = KM30;
                    return KM30;
                }
            }
            
        }
    }

    if (vel_ant == KM30)    /* 80% de chance de ser 60km/h*/
        return (number < 8) ? KM60: KM30; 
    else 
    if (vel_ant == KM60)    /* 60% de chance de ser 60km/h*/
        return (number < 6) ? KM60: KM30;
    
    return KM90; // pro comp. não reclamar
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

int atualiza_Classificacao(pthread_t thread, int * rodada, int * id, int verbose)
{
    Ranking * rank_aux = classThreads;
    struct timespec aux;
    int i, j;

    while(rank_aux != NULL && rank_aux->rodada != *rodada)
        rank_aux = rank_aux->prox;
    
    if (rank_aux == NULL)
        return DELETED;

    // se nesta rodada o número de caras a serem filtrados for 2, a rodada ainda vai ser atualizada
    // até chegar na atualizaVel, então preciso estar na rodada impar cuja filtragem de ciclistas seria 2
    if( rank_aux->rodada%2 == 1 && rank_aux->ideal_ciclistas - 1 - rank_aux->quebrados == 2) {
        if(!finalistas[0]) {
            finalistas[0] = pthread_self();
            printf("Na rodada %d foi decidido que a thread %ld seria uma das finalistas(0)!\n", *rodada, thread);
        }
        else if(!finalistas[1]) {
            finalistas[1] = pthread_self();
            printf("Na rodada %d foi decidido que a thread %ld seria uma das finalistas(1)!\n", *rodada, thread);
        }
        else {
            printf("O ciclista %ld chegou tarde demais na rodada %d !\n", thread, *rodada);
        }

    }
    
    if(assoc[*id][2] == BROKEN)  // agora sim vai entrar aqui
    {
        printf("Ops! O ciclista %4d quebrou na rodada %d! e será eliminado\n", *id, *rodada+1);
        //rank_aux->quebrados++; ->> isso zoa a quebra dos ciclistas

        /* Adiciona na classificação geral */
        general->status[general->ultimo_inserido] = BROKEN;
        general->rodada_tempo[general->ultimo_inserido] = *rodada + 1;
        general->tempo[general->ultimo_inserido] = 0;
        general->t_ranks[general->ultimo_inserido] = thread;
        general->ultimo_inserido++;

        /* Caso quebre e seja o ultimo que tinha que completar a rodada "anterior" */
        if (rank_aux->t_ranks[rank_aux->ideal_ciclistas - 1 - rank_aux->quebrados] && verbose)
        {
            // exibindo ranking da rodada
            printf("\nRanking - Rodada %d:\n", *rodada);
        
            for(int j = 0; j < rank_aux->ideal_ciclistas; j++) 
            printf(" %4do - %04d \n", j+1, findThread(rank_aux->t_ranks[j]));
    
            printf("\n");


            /* Liberando lista de rankings anteriores */
            rank_aux = classThreads;

            if(classThreads->rodada == *rodada) {
                classThreads = classThreads->prox;
                free(rank_aux->t_ranks);
                free(rank_aux);
            }

        }

    }

    /* IMPORTANTE: LEIA ISSO *************************************************************************/
    // mesmo que broken ele tem que rodar o código abaixo na possibilidade de ser o primeiro termo
    // para poder marcar em classThreads qual é o número de elementos quebrados da rodada x

    if(assoc[*id][2] == LATEDELETION)
    {
        printf("Oi! eu, thread %4d, devia ter sido eliminada antes (cheguei em último)...\n", *id);
        // possivelmente imprimir qual rodada que chegou por último.
        return TOBEDELETED;
    }
    else// Caso seja primeiro ciclista a iniciar a rodada
    if (*rodada == maior() && !rank_aux->t_ranks[0])
    {
        printf("%4d EH O PRIMEIRO COLOCADO!!!!!!!\n", *id);

        /* É O VENCEDOR!!!!!!!!!!!!!!!! */
        if (rank_aux->ideal_ciclistas - rank_aux->quebrados == 1)
        {
            printf("%4d EH O VENCEDOR!!!!!!!\n", *id);
            // preciso indicar que ele vai morrer basicamente

            int itr = general->ultimo_inserido;
            int achou = 0;

            /* Adiciona na classificação geral */
            clock_gettime(CLOCK_MONOTONIC, &aux);

            while(itr > -1 && !achou) {
                if(general->status[itr] == DELETED && general->rodada_tempo[itr] < *rodada) 
                    achou = 1;
                else 
                    itr--;
            }

            itr++; // itr precisa estar depois do menor que ele

            for(int i = general->ultimo_inserido; i > itr; i--) {
                general->status[i] = general->status[i-1];
                general->tempo[i] = general->tempo[i-1];
                general->rodada_tempo[i] = general->rodada_tempo[i-1];
                general->t_ranks[i] = general->t_ranks[i-1];
            }

            general->status[itr] = DELETED;
            general->tempo[itr] = (aux.tv_sec - t_start.tv_sec) + (aux.tv_nsec - t_start.tv_nsec)/1000000000.0; /* trocar pelo tempo*/
            general->rodada_tempo[itr] = *rodada;
            general->t_ranks[itr] = thread;

            general->ultimo_inserido++;

            return CHAMPION;
        }

        // posso checar se é o primeiro aqui - o que seria legal ;D

        // SE NÃO FOR QUEBRADO -> tudo bem
        if(assoc[*id][2] != BROKEN) {
            rank_aux->t_ranks[0] = thread;
        }
        // SE FOR QUEBRADO -> atualizo para 0
        else {
            rank_aux->t_ranks[0] = 0;
        }

        // ATUALIZANDO NÚMERO DE QUEBRADOS CASO JÁ TENHA SIDO CRIADO
        if(rank_aux->prox == NULL) {
            /* Reservando espaço para a próxima rodada */
            Ranking * rank_new = (Ranking *) malloc(sizeof(Ranking));
            rank_new->prox = NULL;
            rank_new->ideal_ciclistas = rank_aux->ideal_ciclistas - ((int) (*rodada + 1)%2);
            rank_new->rodada = *rodada + 1;
            rank_new->t_ranks = (pthread_t *) malloc(rank_new->ideal_ciclistas * sizeof(pthread_t));
            rank_new->quebrados = rank_aux->quebrados;

            // zerando todas as posições fora a primeira
            for(j = 0; j < rank_new->ideal_ciclistas; j++)
                rank_new->t_ranks[j] = 0;

            rank_aux->prox = rank_new;

        } 
        // senão já existiu primeiro termo mas foi deletado (então não precisa alocar mais memória)
        // eu não aloco mais, também, porque quero guardar o valor de quebrados

        //printf("Ideal ciclistas: %d  - ranking: %d\n", rank_new->ideal_ciclistas, *rodada + 1);
    }
    else
    {
        /* caso não seja primeiro ciclista a iniciar a rodada */

        /* Encontrando posição livre em t_ranks 
           começa do 1 porque no 0 se encontra o primeiro a passar na rodada */
        for(i = 1;(i < total_ciclistas + 1 - *rodada/2 - rank_aux->quebrados)
               && (rank_aux->t_ranks[i] != 0); i++);

         /* Inserindo na posição correta do vetor t_ranks */
        rank_aux->t_ranks[i] = thread;

    }
    
    /* Caso tenha completado a lista de participantes da rodada,
       exibe classificações */
    if (rank_aux->t_ranks[rank_aux->ideal_ciclistas - 1 - rank_aux->quebrados])
    {
        if (verbose)
        {
            printf("\nRanking - Rodada %d:\n", *rodada);
        
            for(int j = 0; j < rank_aux->ideal_ciclistas - rank_aux->quebrados; j++) 
            printf("%4do - %04d \n", j+1, findThread(rank_aux->t_ranks[j]));
    
            printf("\n");
        }

        /* Liberando lista de rankings anteriores */
        rank_aux = classThreads;

        if(classThreads->rodada == *rodada) {
            classThreads = classThreads->prox;
            free(rank_aux->t_ranks);
            free(rank_aux);
        }

        /*   Se for último elemento de uma rodada par -> será eliminado */
        if (*rodada%2 == 0)
        {
            printf("Ciclista %4d será deletado...\n", *id);

            // Checando se é o penultimo
            if(finalistas[0] == pthread_self() || finalistas[1] == pthread_self()) {
                printf("A VELOCIDADE VAI VOLTAR AO NORMAL COM CERTEZA AGORA!\n");
                tempo = 60000;
                t_ultimos = 0;
                segundo_acabou = 1;
            }

            int itr = general->ultimo_inserido;
            int achou = 0;

            /* Adiciona na classificação geral */
            clock_gettime(CLOCK_MONOTONIC, &aux);

            while(itr > -1 && !achou) {
                if(general->status[itr] == DELETED && general->rodada_tempo[itr] < *rodada) 
                    achou = 1;
                else 
                    itr--;
            }

            itr++; // itr precisa estar depois do menor que ele

            for(int i = general->ultimo_inserido; i > itr; i--) {
                general->status[i] = general->status[i-1];
                general->tempo[i] = general->tempo[i-1];
                general->rodada_tempo[i] = general->rodada_tempo[i-1];
                general->t_ranks[i] = general->t_ranks[i-1];
            }

            general->status[itr] = DELETED;
            general->tempo[itr] = (aux.tv_sec - t_start.tv_sec) + (aux.tv_nsec - t_start.tv_nsec)/1000000000.0; /* trocar pelo tempo*/
            general->rodada_tempo[itr] = *rodada;
            general->t_ranks[itr] = thread;

            general->ultimo_inserido++;

            return TOBEDELETED; 
        }
    }
    
    return ACTIVE;
}

int quebrou() {

    // PRECISO CONSIDERAR QUANTOS QUEBRARAM ANTES DESSE TBM!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1
    // ZERAR QUEBRADOS NO MOMENTO QUE FOR ALOCADO O PRIMEIRO
    // RECEBE O VALOR DE QUEBRADOS DO ANTERIOR
    // AQUI ATUALIZA NÚMERO DE QUEBRADOS PROS PŔOXIMOS

    if(toDestroy != NULL) { // as threads a serem destruídas entram aqui
        Node * d_aux = toDestroy;
        Node * AUX = d_aux;
        Ranking * r_aux = classThreads;

        // DEBUG
        /* printf("\n LISTA A DELETAR: ");
        while(AUX){
            printf("%ld (%d) ", AUX->id%1000, AUX->rodada_pessoal);
            AUX = AUX->prox;
        }

        printf("\n"); */
        //FIM DEBUG

        int ** quebra_rodada = (int **) malloc(total_ciclistas*sizeof(int *));

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
            //assoc[findThread(d_aux->id)][2] = BROKEN;

            j++;
            d_aux = d_aux->prox;
        }

        if(j < total_ciclistas) // -1 para último elemento sinaliza fim da lista de rodadas
            quebra_rodada[j][0] = quebra_rodada[j][1] = -1;

        // INSIRO EM CLASSTHREADS PARA CADA RODADA IDENTIFICADA O NÚMERO DE QUEBRADOS
        j = 0;

        while(r_aux && j < total_ciclistas && quebra_rodada[j][0] != -1) {

            Ranking * itr_rank = r_aux->prox; // para atualizar o número de quebrados dos próximos
            int CONTA = 0;

            if(r_aux->rodada == quebra_rodada[j][0]) {
                r_aux->quebrados += quebra_rodada[j][1];

                while(itr_rank) {
                    itr_rank->quebrados += quebra_rodada[j][1];
                    itr_rank = itr_rank->prox;
                    CONTA++;
                }

                if(!itr_rank) {
                    printf("%d RODADAS ATUALIZADAS!\n", CONTA);
                }

                printf("número de TOTAL (até agora) de quebrados da rodada %d: %d\n", r_aux->rodada, r_aux->quebrados);

                // CHECO SE EXISTE THREAD QUE JÁ PASSOU QUE SERIA A ÚLTIMA (TOBEDELETED)
                int i = total_ciclistas - r_aux->rodada/2 - r_aux->quebrados;

                printf("o número de elementos a passar dessa rodada atualizado: %d\n", i);
                printf("(ou seja: %d (total) - %d (r/2) - %d (queb)\n", total_ciclistas, r_aux->rodada/2, r_aux->quebrados);

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

        for(int i = 0; i < 2; i++)
            free(quebra_rodada[i]);
        
        free(quebra_rodada); 

        // LIBERANDO A LISTA DE DELETADOS
        d_aux = toDestroy;
        AUX = NULL;

        while(d_aux) {
            AUX = d_aux;
            d_aux = d_aux->prox;
            AUX->prox = NULL;
            free(AUX);
        }

        toDestroy = NULL;

        if(!toDestroy)
            printf("LISTA DELETADA 1\n");

        return 1;

    }

    return 0;

}

void mostra_Ranking()
{
    int posicao = 1;

    printf("******************* RANKING ******************* \n\n");
    for(int i = total_ciclistas - 1; i >= 0; i--)
    {
        if (general->status[i] == BROKEN)
        {
            printf("Ciclista: %4d - ", findThread(general->t_ranks[i]));
            printf("Status: Quebrado - Rodada: %3d\n", general->rodada_tempo[i]);
        }
        else {
            printf("Posicao: %4do - ", posicao);
            printf("Ciclista: %04d - ", findThread(general->t_ranks[i]));
            printf("Status: Eliminado - ");
            printf("Rodada: %3d ", general->rodada_tempo[i]);
            printf("Tempo: %.3fs\n", general->tempo[i]);
        }
        
        if (general->status[i] != BROKEN)
            posicao++;
    }

    return;
}

void inicializa_Rankings()
{
    // CLASSTHREADS: inicia a lista de classificação de threads
    classThreads = (Ranking *) malloc(sizeof(Ranking));
    classThreads->t_ranks = (pthread_t *) malloc(total_ciclistas * sizeof(pthread_t));
    classThreads->rodada = 1;
    classThreads->quebrados = total_quebrados;
    classThreads->ideal_ciclistas = total_ciclistas;
    classThreads->prox = NULL;

    // GENERAL: inicia lista de classificação geral das threads
    general = (RankingGeral *) malloc(sizeof(RankingGeral));
    general->status = (int *) malloc(total_ciclistas * sizeof(int));
    general->t_ranks = (pthread_t *) malloc(total_ciclistas * sizeof(pthread_t));
    general->rodada_tempo = (int *) malloc(total_ciclistas * sizeof(int));
    general->tempo = (double *) malloc(total_ciclistas*sizeof(double));
    general->ultimo_inserido = 0;

    for(int i = 0; i < total_ciclistas; i++)
    {
        classThreads->t_ranks[i] = 0;
        general->t_ranks[i] = -1;
        general->status[i] = -1;
        general->rodada_tempo[i] = -1;
        general->tempo[i] = -1;
    }

    return;
}

void inicializa_assoc_pista()
{
    // em assoc[i][0] encontra-se a identificação da thread
    // em assoc[i][1] encontra-se a rodada da respectiva thread
    // em assoc[i][2] encontra-se o status da respectiva thread
    assoc = (pthread_t **) malloc (total_ciclistas * sizeof(pthread_t *));
    pista = (pthread_t **) malloc(tam_pista * sizeof(pthread_t *)); 

    for(int i = 0; i < total_ciclistas; i++)
    {
        assoc[i] = (pthread_t *) malloc (3 * sizeof(pthread_t));

        // as posições inicialmente serão zero
        for(int j = 0; j < 3; j++) 
            assoc[i][j] = 0; // ACTIVE  == 0
    }

    for(int i = 0; i < tam_pista; i++)
    {
        pista[i] = (pthread_t *) malloc(10 * sizeof(pthread_t));

        for(int j = 0; j < 10; j++) 
            pista[i][j] = 0;
    }

    return;
}

void mostra_Pista()
{
    int i, j;
    int pista_ant = 0;
    int pista_aux = 10;

    do
    {
        for(i = pista_ant; i < pista_aux; i++)
        {
            printf(" [%4d]", i + 1);
            if (i+1 % 10 == 0)
                printf("\n");
        }
 
        for(i = 0; i < 10; i++)
        {
            printf("\n");
            for (j = pista_ant; j < (pista_aux); j++)
                if (pista[j][i])
                    printf("  %04d ", findThread(pista[j][i]));
                else
                    printf("       ");
            
        }
        printf("\n");
        
        //debug
        //getchar();
        
        if (tam_pista - pista_aux >= 10)
        {
            pista_ant += 10;
            pista_aux += 10;
        }
        else
        {
            pista_ant += 10;
            pista_aux = tam_pista;
        }
        
        
    } while (pista_ant < tam_pista);
         
    return;
}

void free_program()
{
    for(int i = 0; i < total_ciclistas; i++)
        free(assoc[i]);
    
    free(assoc);

    for(int i = 0; i < tam_pista; i++)
        free(pista[i]);

    free(pista);

    free(general->status);
    free(general->t_ranks);
    free(general->rodada_tempo);
    free(general->tempo);
    free(general);

    free(classThreads->t_ranks);
    free(classThreads);

    return;
}