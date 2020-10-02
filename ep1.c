/* EP1 - Sistemas Operacionais
 * Parte 2 - Threads
 * 
 * Nome: Lara Ayumi Nagamatsu               NUSP: 9910568
 * Nome: Lucy Anne de Omena Evangelista     NUSP: 11221776
 * 
 */

/* ./ep1 <escalonador> <arquivo_trace> <novo_arquivo> <d>
 * sendo <d> opcional! */

/* OBS: TEREMOS QUE CRIAR UM ARQUIVO TXT DE NOME <arg[3]> CONTENDO:
 * NOME TF TR
 * NOME = NOME DO PROCESSO
 * TF = INSTANTE DE TEMPO QUE O PROCESSO TERMINOU
 * TR = TEMPO DE RELÓGIO (TF-T0)
 * E NO FIM UMA LINHA CONTENDO UM ÚNICO NÚMERO QUE INFORMA A
 * QUANTIDADE DE MUDANÇAS DE CONTEXTO EFETUADAS NA SIMULAÇÃO
 * 
 * E SE O USUÁRIO OPTAR POR USAR O COMANDO "d", TEREMOS QUE INDICAR
 * 
 */

/* Bibliotecas */
#include "ep1.h"     /* header */
#include <stdio.h>   /* printf(), fopen()... */
#include <string.h>  /* strlen(), strtok() */
#include <stdlib.h>  /* atoi() */

#include <pthread.h> /* pthread_init(), pthread_mutex_lock()...*/
#include <stdlib.h>
#include <unistd.h>

/* VARIÁVEIS GLOBAIS */
pthread_mutex_t mutex;

int main(int argc, char ** argv)
{
    /* PROCESSOS */
    int num_p;                      // número de processos do arquivo
    Data * processos;               // vetor contendo processos

    /* TIPOS DE ESCALONADOR */
    int escalonador = -1;

    if(argv[2] == NULL) {
        printf("Você precisa inserir um arquivo txt como segundo parâmetro!\n");
        exit(EXIT_FAILURE);
    }

    /* LEITURA DE ARQUIVO + PROCESSOS */
    num_p = contaLinhas(argv[2]);
    processos = (Data *) malloc(num_p*sizeof(Data));
    armazenaProcessos(argv[2], processos);

    for(int i = 0; i < num_p; i++) 
        printf("%s %d %d %d\n", processos[i].processo, processos[i].d0, processos[i].dt, processos[i].deadline);

    printf("\n");

    if(argv[1] != NULL)
        escalonador = atoi(argv[1]);

    /* ESCALANDO PROCESSOS */
    switch(escalonador) {
        case(1):
            printf("ESCALONADOR: First Come, First Served\n");
            FCFS(processos, num_p);
            break;
        case(2):
            printf("ESCALONADOR: Shortest Remaining Time Next\n");
            break;
        case(3):
            printf("ESCALONADOR: Round-Robin\n");
            break;
        default:
            printf("Escalonador não reconhecido.\n");
    }

    /* Liberando memória */
    for(int i = 0; i < num_p; i++)
        free(processos[i].processo);    
    free(processos);

    return 0;
}

int contaLinhas(char * arquivo)
{
    int count = 0;
    char ch;
    FILE * f;

    if ((f = fopen(arquivo, "r")) == NULL) {
      perror("\nHouve um erro ao abrir o arquivo!\n");
      exit(EXIT_FAILURE);
    }

    while(!feof(f)) {
        ch = fgetc(f);
        if(ch == '\n')
            count++;
    }

    fclose(f);
    printf("%d PROCESSOS COMPUTADOS\n", count);

    return count;
}

void armazenaProcessos(char * arquivo, Data * processos)
{
    FILE *f;
    char buf[MAX];      // guarda a string da linha
    char * buf_break;   // guarda a string entre " "
    int i;              // contador de linhas para a posição no vetor
    int j;              // contador dentro da linha a partir da separação " "
    int size;           // tamanho da string

    f = fopen(arquivo, "r");
    i = 0;

    /* abaixo lê as linhas do arquivo */
    while( fgets (buf, MAX, f)!= NULL ) {
        //printf(".%s. tem tamanho %ld, é o processo %d\n", buf, strlen(buf), i);
        
        buf_break = strtok(buf, " ");
        
        j = 0;
        while(buf_break != NULL) {
            switch(j) {
                case 0: // nome do processo
                    size = strlen(buf_break)+1;
                    //printf("%s, tamanho: %d\n", buf_break, size);
                    strcat(buf_break, "\0");
                    processos[i].processo = (char *) malloc(size*sizeof(char));
                    strcpy(processos[i].processo, buf_break);
                    break;
                case 1: // t0
                    processos[i].d0 = atoi(buf_break);
                    break;
                case 2: // dt
                    processos[i].dt = atoi(buf_break);
                    break;
                case 3: // deadline
                    processos[i].deadline = atoi(buf_break);
                    break;
                default:
                    printf("Algo deu errado. :(\n");
                    exit(EXIT_FAILURE);
            };

            buf_break = strtok(NULL, " ");
            j++;
        }

        i++;
    }

    fclose(f);
}

void * existe(void *a)
{
    /* quero receber a duração da thread*/
    /* preciso receber o inicio dela e retornar o final? talvez sim, talvez não o.o*/
    long int duracao = 10; /* convertendo parametro de entrada */
    printf("A duração é de %ld segundos\n", duracao);
    long int i;
    int x = 0;

    /* PROTOCOLO DE ENTRADA*/   
    pthread_mutex_lock(&mutex); // P() -> espera valor de mutex 1 e decrementa
    sleep(1);
    printf("Passou do P()!\n");

    /* REGIÃO CRITICA - EXECUTAR O PROCESSO POR X SEGUNDOS */
    for(i = 0; i < duracao; i++) {
        x++;
        printf("Rodando processo por %ld segundos...", i);
        sleep(1);
    }

    /* PROTOCOLO DE SAIDA */
    pthread_mutex_unlock(&mutex); // V() -> incrementa após P()

    return NULL;
}

void FCFS(Data * processos, int num_p) {
    /* ordena processos prontos em fila por ordem de chegada e executa nessa ordem */

    //int ind = 0;            // índice do processo no vetor processos
    pthread_t tid[10];        // vetor de threads
    //int * num = &num_p;

    pthread_mutex_init(&mutex, NULL); 

    for(int i = 0; i < 10; i++) {
        if (pthread_create(&tid[i], NULL, existe(0), NULL)) {
            printf("\n ERROR creating thread\n");
            exit(1);
        }
    }

    printf("rodou\n");

    /* enquanto não acabou de rodar todos os processos
     * o processo vai rodar por x quantidade de tempo - ou seja, faz operação
     * mas os outros processos tem que ser rodados tbm, em concorrência né.
     * então eu tenho que usar a quantidade de threads disponível. 
     * (inicialmente vou começar com 10 threads mas n lembro se havia necessidade
     * de haver uma quantidade maior ou menor) 
     * insere no arquivo louco lá 
    */

    //pthread_mutex_destroy(&mutex);

}