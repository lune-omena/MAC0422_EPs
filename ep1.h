/* EP1 - Sistemas Operacionais
 * Parte 2 - Threads
 * 
 * Nome: Lara Ayumi Nagamatsu               NUSP: 9910568
 * Nome: Lucy Anne de Omena Evangelista     NUSP: 11221776
 * 
 */

#define MAX 200 // temporário
#define _GNU_SOURCE

/* ESTRUTURAS */
typedef struct data {
    char * processo; 
    int d0, dt, deadline;
} Data;

/* PROTÓTIPOS */
int  contaLinhas(char * arquivo); // Conta o número de linhas do arquivo
void armazenaProcessos(char * arquivo, Data * processos); // Captação de texto em arquivo e armazenamento
void FCFS(Data * processos, int num_p); // Escalonador first come, first serve
void * thread(void *a); // operação de thread pro escalonador FCFS
void SRTN(Data * processos, int num_p); // Escalonador Shortest Job First
void RR(Data * processos, int num_p);