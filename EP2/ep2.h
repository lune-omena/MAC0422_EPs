/******************************************************************************************************
 * EP2 - CICLISTAS (HEADER)
 * 
 * Nome: Lara Ayumi Nagamatsu             NUSP: 9910568
 * Nome: Lucy Anne de Omena Evangelista   NUSP: 11221776
 * 
 *****************************************************************************************************/

/* BIBLIOTECAS */
#include <pthread.h>        /* POSIX threads */

/* FUNÇÕES */
// função chamada pela thread assim que ela é criada
void * thread(void * a);
// função INICIAL que insere o ciclista numa posição decente
int  insereNaPista(pthread_t thread);
// possivelmente função usada para atualizar posição do ciclista
int atualizaPos(pthread_t thread, int pos_i, int pos_j);