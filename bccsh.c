/* EP1 - Sistemas Operacionais
 * 
 * Nome: Lara Ayumi Nagamatsu               NUSP: 9910568
 * Nome: Lucy Anne de Omena Evangelista     NUSP: 
 * 
 */

#define MAX 100 // número de caracteres a serem lidos por linha no bccsh

/* A compilação do código deve gerar dois binários. Um binário do bccsh e um binário do simulador de
processos (ep1). */

/* Bibliotecas */
#include <stdio.h>      /* printf(), fgets()... */
#include <stdlib.h>     /* malloc() */
#include <unistd.h>     /* sleep()*/
#include <pthread.h>    /* usado para threads e semáforos */

/* Execução */
int main () {

    char * buffer;
    buffer = (char *) malloc(MAX*sizeof(char));

    printf("bom dia\n");
    printf("Digite CTRL+C para finalizar.\n");

    /* Código retirado da aula de 17/09 */
    while (1) {
        // type_prompt();
        printf("lara@salsinha:~/bla$ ");
        // read_command(command, parameters);
        fgets(buffer, MAX, stdin);

        /*
        if(fork() != 0) {
            // Código do pai
            waitpid(-1, &status, 0);
        }
        else {
            // Código do filho
            execve(command, parameters, 0);
        }
        */

       //printf("\n%s\n", buffer);
       //sleep(1);
    }

    free(buffer);
    
    return 0;
}