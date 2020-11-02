/******************************************************************************************************
 * EP2 - CICLISTAS (HEADER)
 * 
 * Nome: Lara Ayumi Nagamatsu             NUSP: 9910568
 * Nome: Lucy Anne de Omena Evangelista   NUSP: 11221776
 * 
 *****************************************************************************************************/

/* BIBLIOTECAS */
#include <pthread.h>        /* POSIX threads */

enum status {ACTIVE, DELETED, TOBEDELETED, BROKEN};

// a estrutura abaixo era usada para remoção - provavelmente vou recicl=a-la
typedef struct node {
    pthread_t id;
    int i; // posição em relação a d na pista
    int j; // posição em relação às 10 posições dentro de cada d da pista
    int rodada_pessoal; // número da rodada pessoal
    struct node * prox;
} Node;

// a estrutura abaixo será usada para guardar uma lista ligada de classificações
// das pistas
typedef struct rank {
    int rodada;              // número da rodada para classificações
    int quebrados;           // número de ciclistas que quebraram
    pthread_t * t_ranks;     // classificações
    struct rank * prox;      // aponta para próxima rodada
} Ranking;

    //bla
/* FUNÇÕES */
// função chamada pela thread assim que ela é criada
void * thread(void * a);
// função INICIAL que insere o ciclista numa posição decente
int insereNaPista(pthread_t thread);
// função usada para atualizar posição do ciclista
int atualizaPos(pthread_t thread, int pos_i, int *pos_j, int *rodada, int *vel_atual);
// função usada para atualizar velocidade do ciclista
int atualizaVel(int vel_ant, int volta);
// função que atualiza o número da rodada dado pelo vetor assoc (retorna 1 se pode mudar, 0 se não)
int atualizaRodada(pthread_t thread, int rodada, int n);
// função que cospe posição da thread no vetor asoc
int findThread(pthread_t thread);
// função que retorna a rodada do primeiro colocado
int maior();

/* funções a serem implementadas */
//registraPosicao(pthread_self(), /* parametros para registro */);
//verificaQuebra(pthread_self(), /* mesmos parametros para registrar posição */);