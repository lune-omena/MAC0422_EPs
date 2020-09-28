/* EP1 - Sistemas Operacionais
 * Parte 2 - Threads
 * 
 * Nome: Lara Ayumi Nagamatsu               NUSP: 9910568
 * Nome: Lucy Anne de Omena Evangelista     NUSP: 11221776
 * 
 */

/* A invocação do simulador no bccsh deve receber como primeiro parâmetro obrigatório o número
representando cada escalonador, conforme a listagem acima, como segundo parâmetro obrigatório o
nome do arquivo de trace e como terceiro parâmetro obrigatório o nome de um arquivo que será criado
pelo simulador com 1 linha para cada processo e mais 1 linha extra no final. */

/* aqui vai vir o código das threads :)*/
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>  /* atoi() */

int main(int argc, char ** argv) {

    /* arquivo de trace */
    FILE *arquivo;

    arquivo = fopen(argv[2], "r");

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

    printf("%s\n", argv[0]);

    printf("batata\n");
    sleep(1);
    printf("jabuticaba\n");
        
    return 0;
}