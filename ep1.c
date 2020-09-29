/* EP1 - Sistemas Operacionais
 * Parte 2 - Threads
 * 
 * Nome: Lara Ayumi Nagamatsu               NUSP: 9910568
 * Nome: Lucy Anne de Omena Evangelista     NUSP: 11221776
 * 
 */

#define MAX 200 // temporário

/* ./ep1 <escalonador> <arquivo_trace> <novo_arquivo> <d>
 * sendo <d> opcional! */

/* Estruturas */
typedef struct data {
    char * processo;
    int d0, dt, deadline;
} Data;

/* Funções */
// Conta o número de linhas do arquivo
int  contaLinhas(char * arquivo);
// Captação de texto em arquivo e armazenamento
void armazenaProcessos(char * arquivo, Data * processos);

/* Bibliotecas */
#include <stdio.h>
#include <unistd.h>
#include <string.h>  /* strlen(), strtok() */
#include <stdlib.h>  /* atoi() */

int main(int argc, char ** argv) {

    /* PROCESSOS */
    int num_p;                      // número de processos do arquivo
    Data * processos;               // vetor contendo processos

    /* TIPOS DE ESCALONADOR */
    int escalonador = -1;
    
    if(argv[1] != NULL)
        escalonador = atoi(argv[1]);

    switch(escalonador) {
            case(1):
                printf("First Come, First Served\n");
                break;
            case(2):
                printf("Shortest Remaining Time Next\n");
                break;
            case(3):
                printf("Round-Robin\n");
                break;
            default:
                printf("Escalonador não reconhecido.\n");
    }

    if( argv[2] != NULL ) {
        /* Lendo o arquivo e consequentemente os processos */
        num_p = contaLinhas(argv[2]);
        processos = (Data *) malloc(num_p*sizeof(Data));

        armazenaProcessos(argv[2], processos);
        printf("%d e %d e %d \n", processos[0].d0, processos[0].dt, processos[0].deadline); //teste
        printf("%s\n", processos[0].processo); //teste

        printf("%d e %d e %d \n", processos[1].d0, processos[1].dt, processos[1].deadline); //teste
        printf("%s\n", processos[1].processo); //teste
    }

    printf("batata\n");
    sleep(1);
    printf("jabuticaba\n");

    /* Liberando memória */
    if(argv[2])
        free(processos);
        
    return 0;
}

int contaLinhas(char * arquivo) {
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
    printf("%d processos computados\n", count);

    return count;
}

void armazenaProcessos(char * arquivo, Data * processos) {
    FILE *f;
    char buf[MAX];
    char * buf_break;
    int i;              // contador de linhas para a posição no vetor
    int j;              // contador dentro da linha a partir da separação " "

    f = fopen(arquivo, "r");
    i = 0;

    /* abaixo lê as linhas do arquivo */
    while( fgets (buf, MAX, f)!= NULL ) {
      /* writing content to stdout */
        //puts(buf);
        printf(".%s. tem tamanho %ld, é o processo %d\n", buf, strlen(buf), i);
        //aparentemente o fgets pega até o \n, então tem que retirar
        
        buf_break = strtok(buf, " ");
        
        j = 0;
        while(buf_break != NULL) {
            switch(j) {
                case 0:
                    processos[i].processo = buf_break;
                    printf("caso 0\n");
                    break;
                case 1:
                    processos[i].d0 = atoi(buf_break);
                    printf("caso 1\n");
                    break;
                case 2:
                    processos[i].dt = atoi(buf_break);
                    printf("caso 2\n");
                    break;
                case 3:
                    processos[i].deadline = atoi(buf_break);
                    printf("caso 3\n");
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

/*
Data ** armazenaProcessos(char * arquivo)
{
    Data *processos;
    FILE *arq, *count;
    char ch;
    char * proces;
    //char array[50];
    //int d0, dt, deadline;
    int linhas = 0, i = 0;

    count = fopen(arquivo, "r"); 

    if (count == NULL)
    {
      perror("\nHouve um erro ao abrir o arquivo!\n");
      exit(EXIT_FAILURE);
    }

    while(!feof(count))
    {
        ch = fgetc(count);
        if(ch == '\n')
            linhas++;
        
    }
    fclose(count);

    printf("\nNós temos %d linhas no arquivo", linhas);
    processos = (Data * )malloc(linhas * sizeof(Data));
    
    processos[0].processo = "ola";
    processos[0].d0 = 3;
    processos[0].dt = 4;
    processos[0].deadline = 20;
    processos[1].processo = "o121la";
    processos[1].d0 = 7;
    processos[1].dt = 9;
    processos[1].deadline = 15;
    printf("\n%s %d %d %d", processos[0].processo, processos[0].d0, processos[0].dt, processos[0].deadline); 
    printf("\n%s %d %d %d", processos[1].processo, processos[1].d0, processos[1].dt, processos[1].deadline); 
    printf("\nRolou?\n");
    printf("\nArquivo:");
    i = 0;
    arq = fopen(arquivo, "r"); 

     while(!feof(arq) && i < linhas)
    {   
        if (i < linhas)
        {
            fscanf(arq, "%s %d %d %d", processos[i].processo, &processos[i].d0, &processos[i].dt, &processos[i].deadline);
            printf("\n%s %d %d %d", processos[i].processo, processos[i].d0, processos[i].dt, processos[i].deadline);
            FUNCIONA QUANDO USA VARIAVEIS LOCAIS 
            fscanf(arq, "%s %d %d %d", proces, &d0, &dt, &deadline);
            i++;
        }

        fscanf(arq, "%s", proces);

       
        printf("\nUé"); 
    } 



    printf("\nRolou???????????????\n");
    fclose(arq);

    return NULL;
}

*/