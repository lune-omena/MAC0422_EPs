/* EP1 - Sistemas Operacionais
 * 
 * Nome: Lara Ayumi Nagamatsu               NUSP: 9910568
 * Nome: Lucy Anne de Omena Evangelista     NUSP: 
 * 
 */

/* A compilação do código deve gerar dois binários. Um binário do bccsh e um binário do simulador de
processos (ep1). */

/* Bibliotecas */
#include <stdio.h>      /* printf(), scanf()... */
#include <unistd.h>     /* sleep()*/
#include <pthread.h>    /* usado para threads e semáforos */

/* Execução */
int main () {

    printf("bom dia\n");
    printf("Digite CTRL+C para finalizar.\n");

    /* Código retirado da aula de 17/09 */
    while (1) {
        // type_prompt();
        // read_command(command, parameters);

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
    }

    return 0;
}