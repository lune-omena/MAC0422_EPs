/* EP1 - Sistemas Operacionais
 * Parte 1 - SHELL
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
#define INVAL_OP "Operação inválida.\n"
#define MAX 200

/* A compilação do código deve gerar dois binários. Um binário do bccsh e um binário do simulador de
processos (ep1). */

/* Bibliotecas */
#include <stdio.h>               /* printf(), fgets()... */
#include <stdlib.h>              /* malloc(), free() */
#include <unistd.h>              /* sleep(), getlogin(), SYS_write */
#include <sys/syscall.h>         /* usado para syscalls */
#include <sys/wait.h>            /* waitpid() */
#include <readline/readline.h>   /* ler linha de comando */
#include <readline/history.h>    /* historico do terminal */
#include <string.h>              /* strcmp(), strtok()... */
#include <sys/stat.h>            /* syscall mkdir, kill, ...*/
#include <fcntl.h>               /* AT_FDCWD do ln -s ... */   

/* Execução */
int main () {

    /* PROMPT/TEXTO */
    char * buffer;                              // buffer de texto
    //char * usr = getlogin();                    // pega o nome de usuário
    char * prompt = "{daniel@/tmp/mac0422/} "; 
    char * buf_break;

    /* PROCESSOS */
    pid_t childpid;  // usado para processo filho
    int opcao = -1;  // identificar se é por syscall ou invocação externa
    char * args[4];  // usados como parâmetros para execve obs: último valor de args é NULL

    using_history();

    printf("bom dia\n");
    printf("Digite CTRL+D para finalizar.\n");

    /* Código retirado da aula de 17/09 */
    while ((buffer=readline(prompt))) {

        opcao = -1;

        /* COMANDO EXTERNO: /usr/bin/du -hs . */
        if(!strcmp(buffer, DU_CMD)) {

            opcao = 0;
            args[0] = "/usr/bin/du";
            args[1] = "-hs";
            args[2] = ".";
            args[3] = NULL;

        }
        /* COMANDO EXTERNO: /usr/bin/traceroute www.google.com.br */
        else if(!strcmp(buffer, TRCRT_CMD)) {

            opcao = 0;
            args[0] = "/usr/bin/traceroute";
            args[1] = "www.google.com.br";
            args[2] = NULL;

        }
        else {

            buf_break = strtok(buffer, " ");

            /* roda caso usuário não inserir nada */
            if(buf_break == NULL) {
                args[0] = NULL;
            }
            /* COMANDO EXTERNO: ./ep1 <argumentos do EP1> */
            else if(!strcmp(buf_break, "./ep1")) {
                
                opcao = 0;
                args[0] = "./ep1";
                /* não sei quantos argumnetos vai ter, agr só vou aeitar qu tem 0*/
                args[1] = NULL;

            }
            /* COMANDO INTERNO: mkdir <diretorio> */
            else if(!strcmp(buf_break, "mkdir")) {
                char * dirname = strtok(NULL, " ");
                
                if(dirname == NULL) 
                    printf("Precisa de mais argumentos.\n");

                else if(!mkdir(dirname,0777)) 
                    printf("Criado o diretório de nome %s.\n", dirname);

                else 
                    printf("Não foi possível criar o diretório.\n");
            }
            /* COMANDO INTERNO: kill -9 <PID> */
            else if(!strcmp(buf_break, "kill")) {
                buf_break = strtok(NULL, " ");

                if(buf_break!= NULL && !strcmp(buf_break, "-9")) {
                    buf_break = strtok(NULL, " ");

                    if(buf_break != NULL) {
                        long int kill_pid = strtol(buf_break, NULL, 0);

                        if(!kill(kill_pid,SIGKILL)) 
                            printf("Processo %ld deletado\n", kill_pid);
                        else
                            printf("Não foi possível deletar o processo %ld.\n", kill_pid);
                    }
                    else
                        printf(INVAL_OP);
                }
                else 
                    printf(INVAL_OP);
            }
            /* COMANDO INTERNO: ln -s <arquivo> <link> */
            else if(!strcmp(buf_break, "ln")) {
                buf_break = strtok(NULL, " ");

                if(buf_break != NULL && !strcmp(buf_break, "-s")) {
                    char * arquivo = strtok(NULL, " ");

                    if( arquivo != NULL ) {
                        char * link = strtok(NULL, " ");

                        if( link != NULL ) {
                            if(!symlinkat(arquivo, AT_FDCWD, link))
                                printf("Atalho %s para %s foi criado.\n", link, arquivo);
                            else
                                printf("Não foi possíve criar o atalho %s para %s\n", link, arquivo);

                        }
                        else
                            printf(INVAL_OP);
                    }
                    else
                        printf(INVAL_OP);
                }
                else
                    printf(INVAL_OP);
            }
            else {
                printf("Comando não identificado.\n");
                //opcao = -1;
                args[0] = NULL;
            }

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