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

struct processo {

};

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
         * ou podemos guardar um vetor com o nome dos processos, um vetor pro t0 e outro pro dt 
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