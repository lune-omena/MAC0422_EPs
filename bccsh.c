/* EP1 - Sistemas Operacionais
 * 
 * Nome: Lara Ayumi Nagamatsu               NUSP: 9910568
 * Nome: Lucy Anne de Omena Evangelista     NUSP: 11221776
 * 
 */

/* LUCY!!!! gostei muito deste site: https://explainshell.com/explain?cmd=du+-hs+.
 * dê uma olhada :) */

#define MAX 100 // número de caracteres a serem lidos por linha no bccsh

/* A compilação do código deve gerar dois binários. Um binário do bccsh e um binário do simulador de
processos (ep1). */

/* o bccsh deve permitir a execução (externa) de:
 * 
 * /usr/bin/du -hs .                        -> https://www.geeksforgeeks.org/du-command-linux/
 * (o comando du mostra o espaço de disco utilizado pelos files)
 * /usr/bin/traceroute www.google.com.br    -> https://www.geeksforgeeks.org/traceroute-command-in-linux-with-examples/
 * (traceroute imprime a rota que um pacote faz até chegar ao host)
 * ./ep1 <argumentos do EP1>
 * (este é o simulador de processos (: )
 * 
 * (obrigatoriamente usando syscalls que não sejam da famı́lia exec* ou similares.)
 * 
 * e também (interna) de:
 * 
 * mkdir <diretorio>
 * kill -9 <PID>
 * ln -s <arquivo> <link>
 * 
 */

/* Bibliotecas */
#include <stdio.h>          /* printf(), fgets()... */
#include <stdlib.h>         /* malloc() */
#include <unistd.h>         /* sleep()*/
#include <pthread.h>        /* usado para threads e semáforos */
#include <unistd.h>         /* contém constantes mágicas do unix, ex: SYS_write */
#include <sys/syscall.h>    /* usado para syscalls */
#include <sys/wait.h>       /* waitpid()*/

//typedef long int pid_t;

/* Execução */
int main () {
    //https://stackoverflow.com/questions/30149779/c-execve-parameters-spawn-a-shell-example

    char * buffer;   // buffer de texto
    pid_t childpid;  // usado para processo filho
    char * args[2];  // usados como parâmetros para execve

    // Vamos ter que mudar isso aqui :0
    args[0] = "/bin/ls";
    //args[1] = "-lh";
    args[1] = NULL;

    buffer = (char *) malloc(MAX*sizeof(char));

    printf("bom dia\n");
    printf("Digite CTRL+C para finalizar.\n");

    /* Código retirado da aula de 17/09 */
    while (1) {
        // type_prompt();
        printf("lara@salsinha:~/bla$ ");
        // read_command(command, parameters);
        fgets(buffer, MAX, stdin);

        if( (childpid = fork()) == 0 ) {
            // Código do filho
            printf("código do filho\n");
            //execve(command, parameters, 0);
            execve(args[0], args, NULL);
            /*  aparentemente o execve funciona da seguinte maneira:
                o primeiro argumento é o processo a substituir o atual (com seu path)
                o segundo indica os argumentos que seguiriam em seguida do path (tipo argc da main) ->
                (By convention, the  first of  these  strings  (i.e.,  argv[0])  should contain the filename 
                associated with the file being executed. )
                o terceiro é um array de strings ? parece que pode ser 0 ou NULL;
                ( The argv and envp arrays must each include a null pointer at the end of the array.)
            */

            //(https://man7.org/linux/man-pages/man2/execve.2.html)

            while (1) {
                sleep(1);
                printf("Primeiro processo filho...\n");
            }
        }
        else {
            // Código do pai
            printf("código do pai\n");
            //waitpid(-1, &status, 0);
            waitpid(-1, NULL, 0);
            //(https://man7.org/linux/man-pages/man2/waitpid.2.html)
        }

       //printf("\n%s\n", buffer);
       //sleep(1);
    }

    free(buffer);

    return 0;
}