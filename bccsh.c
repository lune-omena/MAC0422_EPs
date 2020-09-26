/* EP1 - Sistemas Operacionais
 * 
 * Nome: Lara Ayumi Nagamatsu               NUSP: 9910568
 * Nome: Lucy Anne de Omena Evangelista     NUSP: 11221776
 * 
 */

/* LUCY!!!! gostei muito deste site: https://explainshell.com/explain?cmd=du+-hs+.
 * dê uma olhada :) */

// PARA FACILITAR ENTENDIMENTO
#define DU_CMD "/usr/bin/du -hs ." 
#define TRCRT_CMD "/usr/bin/traceroute www.google.com.br"

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
#include <stdio.h>               /* printf(), fgets()... */
#include <stdlib.h>              /* malloc(), free() */
#include <unistd.h>              /* sleep(), contém constantes mágicas do unix, ex: SYS_write */
#include <pthread.h>             /* usado para threads e semáforos */
#include <sys/syscall.h>         /* usado para syscalls */
#include <sys/wait.h>            /* waitpid() */
#include <readline/readline.h>   /* ler linha de comando */
#include <readline/history.h>    /* historico do terminal */
#include <string.h>              /* strcmp()... */
//obs: tive que instalar a lib readline. isso aconteceu com vc tbm?

/* Execução */
int main () {
    //https://stackoverflow.com/questions/30149779/c-execve-parameters-spawn-a-shell-example

    char * buffer;   // buffer de texto
    char * prompt = "lara@salsinha:~/bla$ "; // usuário :)
    pid_t childpid;  // usado para processo filho
    int opcao = -1;

    char * args[4];  // usados como parâmetros para execve obs: último valor de args é NULL

    using_history();

    printf("bom dia\n");
    printf("Digite CTRL+C para finalizar.\n");

    /* Código retirado da aula de 17/09 */
    while ((buffer=readline(prompt))) {

        /* COMANDOS DA BCCSH */
        if(!strcmp(buffer, DU_CMD)) {
            opcao = 0;
            args[0] = "/usr/bin/du";
            args[1] = "-hs";
            args[2] = ".";
            args[3] = NULL;
        }
        else if(!strcmp(buffer, TRCRT_CMD)) {
            opcao = 0;
            args[0] = "/usr/bin/traceroute";
            args[1] = "www.google.com.br";
            args[2] = NULL;
        }
        // EP1
        else {
            printf("Comando não identificado.\n");
            opcao = -1;
            args[0] = NULL;
        }
        
        // type_prompt();
        // read_command(command, parameters);

        // aqui vai vir o código das 3 que envolvem syscalls
        if(!opcao) {
            if( (childpid = fork()) == 0 ) {
                // Código do filho
                printf("código do filho\n");
                execve(args[0], args, NULL);
                /*  aparentemente o execve funciona da seguinte maneira:
                    o primeiro argumento é o processo a substituir o atual (com seu path)
                    o segundo indica os argumentos que seguiriam em seguida do path (tipo argc da main) ->
                    (By convention, the  first of  these  strings  (i.e.,  argv[0])  should contain the filename 
                    associated with the file being executed. )
                    o terceiro é um array de strings ? parece que pode ser 0 ou NULL;
                    ( The argv and envp arrays must each include a null pointer at the end of the array.)
                    (https://man7.org/linux/man-pages/man2/execve.2.html)  */
            }
            else {
                // Código do pai
                printf("Sou o processo pai, criei o processo %d\n", childpid);
                waitpid(-1, NULL, 0);
                /* devolve controle ao processo pai quando todos os processo filhos morrerem 
                https://man7.org/linux/man-pages/man2/waitpid.2.html) */
            }
        }

       add_history(buffer);
       free(buffer);

    }

    return 0;
}