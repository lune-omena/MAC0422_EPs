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
/*
struct utsname unameData;

typedef struct data {
    char * processo;
    int d0, dt, deadline;
} Data;
*/

/* Funções */
// Captação de texto em arquivo e armazenamento
//Data ** armazenaProcessos(char * arquivo);

/* aqui vai vir o código das threads :)*/
#include <stdio.h>
#include <unistd.h>
#include <string.h>  /* strlen() */
#include <stdlib.h>  /* atoi() */

int main(int argc, char ** argv) {

    /* arquivo de trace */
    FILE *arquivo;
    //char * buf;
    /* nome é uma string sem espaços em branco de no máximo 30 caracteres: 30 + */
    char buf[MAX];



    //Data **processos;

    /* LEITURA DE ARQUIVO  - COM PROBLEMA, PFF COMENTAR*/
  /*   processos = armazenaProcessos("teste.txt"); */

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

    arquivo = fopen(argv[2], "r");

    /* abaixo lê as linhas do arquivo */
    while( fgets (buf, MAX, arquivo)!=NULL ) {
      /* writing content to stdout */
        puts(buf);
        printf("Tamanho de %s é %ld\n", buf, strlen(buf));
        /* na aula, o professor falou de 2 casos: poderiamos ler todo o arquivo e acionar as threads nos tempos dados
         * ou poderíamos ir acionando as threads conforme recebemos os processos. 
         * se usarmos a primeira opção:
         * eu não sei como vamos guardar as informações, mas cada linha possui um processo:
         * nome t0 dt deadline
         * o nome tem no máximo 30 caracteres, os outros são ints.
         * acho legal usarmos structs para montar o recebimento desses dados, gostaria de saber oq vc acha :)
         * ou podemos guardar um vetor com o nome dos processos, um vetor pro t0 e outro pro dt ]
         * (eu estou votando pelo que for mais fácil de implementar, sinceramente x.x)
         * se for pela segunda:
         * não precisaríamos guardar tudo num vetor de cara
         * mas nai sei :0*/
    }

    fclose(arquivo);    

    printf("batata\n");
    sleep(1);
    printf("jabuticaba\n");
        
    return 0;
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

    /* processos[0].processo = "ola";
    processos[0].d0 = 3;
    processos[0].dt = 4;
    processos[0].deadline = 20;
    processos[1].processo = "o121la";
    processos[1].d0 = 7;
    processos[1].dt = 9;
    processos[1].deadline = 15;
    printf("\n%s %d %d %d", processos[0].processo, processos[0].d0, processos[0].dt, processos[0].deadline); 
    printf("\n%s %d %d %d", processos[1].processo, processos[1].d0, processos[1].dt, processos[1].deadline); */ 
    /*
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
            /* FUNCIONA QUANDO USA VARIAVEIS LOCAIS */
            /*  fscanf(arq, "%s %d %d %d", proces, &d0, &dt, &deadline); */
            /*i++;
        }

        fscanf(arq, "%s", proces);

       
        printf("\nUé"); 
    } 



    printf("\nRolou???????????????\n");
    fclose(arq);

    return NULL;
}

*/