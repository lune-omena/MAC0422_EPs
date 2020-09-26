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
#include <string.h>              /* strcmp(), strtok()... */

#include <sys/stat.h>            /* syscall mkdir, kill, ...*/
//obs: tive que instalar a lib readline. isso aconteceu com vc tbm?

/* Execução */
int main () {
    //https://stackoverflow.com/questions/30149779/c-execve-parameters-spawn-a-shell-example

    char * buffer;   // buffer de texto
    char * prompt = "{daniel@/tmp/mac0422/} "; // usuário :)
    pid_t childpid;  // usado para processo filho
    int opcao = -1;  // identificar se é por syscall ou invocação externa
    char * args[4];  // usados como parâmetros para execve obs: último valor de args é NULL

    /* teste */
    char* dirname = "novo";
    /* fim teste */

    using_history();

    printf("bom dia\n");
    printf("Digite CTRL+C para finalizar.\n");

    /* Código retirado da aula de 17/09 */
    while ((buffer=readline(prompt))) {

        opcao =  -1;

        /* COMANDOS EXTERNOS */
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
        /* FALTA SÓ ESTE: EP1 */

        /* COMANDOS INTERNOS: */ // aqui vai vir o código das 3 que envolvem syscalls
        // aqui vou ter que quebrar a string buffer por meio dos espaços
        // usarei o comando strtok() para quebrar a string vinda do buffer
        else if(!strcmp(buffer, "hola")) {
            /* é bem estranho, mas parece que a syscall do mkdir é mkdir o.o*/
            if(!mkdir(dirname,0777))
                printf("Criado.\n");
            else
                printf("Não foi possível criar o diretório.\n"); 
        }
        else if(!strcmp(buffer, "aiai")) {
            if(!kill(2229,SIGKILL)) // o primeiro argumento é o pid do processo! mudar dps
                printf("Processo %d deletado\n", 2229);
            else
                printf("Não foi possível deletar o processo %d.\n", 2229);
        }
        else if(!strcmp(buffer, "buuu")) {

            /*  stat("tx2.txt", 0x7ffc670e92a0)         = -1 ENOENT (No such file or directory)
                symlinkat("tx1.txt", AT_FDCWD, "tx2.txt") = 0
                lseek(0, 0, SEEK_CUR)                   = -1 ESPIPE (Illegal seek)
            */
        }
        else {
            printf("Comando não identificado.\n");
            //opcao = -1;
            args[0] = NULL;
        }


        /* Execução (invocação externa: */
        if(!opcao) {
            if( (childpid = fork()) == 0 ) {
                // Código do filho
                execve(args[0], args, NULL);
                // obs: último valor de args é necessáriamente NULL
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