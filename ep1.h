/* EP1 - Sistemas Operacionais
 * Parte 2 - Threads
 * 
 * Nome: Lara Ayumi Nagamatsu               NUSP: 9910568
 * Nome: Lucy Anne de Omena Evangelista     NUSP: 11221776
 * 
 */

#define MAX 200 // temporário

/* ESTRUTURAS */
typedef struct data {
    char * processo; 
    int d0, dt, deadline;
} Data;

/* PROTÓTIPOS */
int  contaLinhas(char * arquivo); // Conta o número de linhas do arquivo
void armazenaProcessos(char * arquivo, Data * processos); // Captação de texto em arquivo e armazenamento
void FCFS(Data * processos, int num_p);
void * thread(void *a);