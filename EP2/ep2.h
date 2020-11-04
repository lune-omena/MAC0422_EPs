/******************************************************************************************************
 * EP2 - CICLISTAS (HEADER)
 * 
 * Nome: Lara Ayumi Nagamatsu             NUSP: 9910568
 * Nome: Lucy Anne de Omena Evangelista   NUSP: 11221776
 * 
 *****************************************************************************************************/

/* BIBLIOTECAS */
#include <pthread.h>        /* POSIX threads */

enum status {ACTIVE, DELETED, TOBEDELETED, BROKEN, LATEDELETION, CHAMPION};

// a estrutura abaixo era usada para remoção - provavelmente vou recicla-la
typedef struct node {
    pthread_t id;
    //int i; // posição em relação a d na pista
    //int j; // posição em relação às 10 posições dentro de cada d da pista
    int rodada_pessoal; // número da rodada pessoal
    struct node * prox;
} Node;

// a estrutura abaixo será usada para guardar uma lista ligada de classificações
// das pistas
typedef struct rank {
    int rodada;              // número da rodada para classificações
    int quebrados;           // número de ciclistas que quebraram
    int ideal_ciclistas;     // número de ciclistas ideais para a rodada
    pthread_t * t_ranks;     // classificações
    struct rank * prox;      // aponta para próxima rodada
} Ranking;

typedef struct rankG {
    int * status;            // armazena status ao sair da corrida
    pthread_t * t_ranks;     // classificações gerais dos ciclistas - de traz para frente
    int ultimo_inserido;     // posicao do ultimo inserido na lista
    int * rodada_tempo;         // armazena tempo ou rodada de quebra
    double * tempo;          // armazena tempo da quebra
} RankingGeral;


/* FUNÇÕES */
// função chamada pela thread assim que ela é criada
void * thread(void * a);
// função INICIAL que insere o ciclista numa posição decente
int insereNaPista(pthread_t thread);
// função usada para atualizar posição do ciclista
int atualizaPos(pthread_t thread, int pos_i, int *pos_j, int *rodada, int *vel_atual, int * id);
// função usada para atualizar velocidade do ciclista
int atualizaVel(int vel_ant, int volta, pthread_t t);
// função que atualiza o número da rodada dado pelo vetor assoc (retorna 1 se pode mudar, 0 se não)
int atualizaRodada(pthread_t thread, int rodada, int n);
// função que cospe posição da thread no vetor asoc
int findThread(pthread_t thread);
// função que retorna a rodada do primeiro colocado
int maior();
// função para gerenciar quebra de ciclistas
int quebrou();
/* Função para atualizar rankeamento a cada rodada finalizada e exibir ranking */
int atualiza_Classificacao(pthread_t thread, int * rodada, int * id, int verbose);

void inicializa_Rankings();

void inicializa_assoc_pista();

void mostra_Ranking();

void mostra_Pista();

void free_program();

/* funções a serem implementadas */
//verificaQuebra(pthread_self(), /* mesmos parametros para registrar posição */);