/* EP3 - Sistemas Operacionais
 * 
 * Nome: Lara Ayumi Nagamatsu               NUSP: 9910568
 * Nome: Lucy Anne de Omena Evangelista     NUSP: 11221776
 * 
 */

/* Bibliotecas */
#include "ep3.h"               /* header */
#include <stdio.h>               /* printf(), fgets()... */
#include <stdlib.h>              /* malloc(), free() */

#include <sys/syscall.h>         /* usado para syscalls */
#include <sys/stat.h>            /* syscall mkdir, kill, ...*/

#include <readline/readline.h>   /* ler linha de comando */
#include <readline/history.h>    /* historico do terminal */
#include <string.h>              /* strcmp(), strtok()... */

/* Funções a serem implementadas:
- mount arquivo
- cp origem destino:
- mkdir diretorio
- rmdir diretorio
- cat arquivo
- touch arquivo
- rm arquivo
- ls diretorio
- find diretorio arquivo
- df
- umount
- sai
*/
int main ()
{
    /* auxiliares para o terminal */
    char * buffer;                                
    char * prompt; 
    char * buf_break;
    /* argumentos usados como parâmetros para as funcoes cp e find */
    char * args[2];  
    
    /* FORMATAÇÃO DO PROMPT */
    prompt = definePrompt();

    using_history();

    printf("Digite CTRL+D para finalizar.\n");

    while ((buffer = readline(prompt)))
    {
        add_history(buffer);

        buf_break = strtok(buffer, " ");

        /* roda caso usuário não inserir nada */
        if(buf_break == NULL) {
            args[0] = NULL;
        }
        /* PRECISA REALIZAR FUNCAO */
        else if(!strcmp(buf_break, "mount")) {
            char * dirname = strtok(NULL, " ");
            
            /* if(dirname == NULL) 
                printf("Precisa de mais argumentos.\n");

            else if(!mkdir(dirname,0777)) 
                printf("Criado o diretório de nome %s.\n", dirname);

            else 
                printf("Não foi possível criar o diretório.\n"); */
        }
        else if(!strcmp(buf_break, "umount"))
        {
            char * dirname = strtok(NULL, " ");
            
            /* if(dirname == NULL) 
                printf("Precisa de mais argumentos.\n");

            else if(!mkdir(dirname,0777)) 
                printf("Criado o diretório de nome %s.\n", dirname);

            else 
                printf("Não foi possível criar o diretório.\n"); */
        }
        /* PRECISA REALIZAR FUNCAO */
        else if(!strcmp(buf_break, "cp"))
        {
            int i;

            for(i = 0; buf_break != NULL && i < 2; i++) { // origem e destino
                args[i] = buf_break;
                buf_break = strtok(NULL, " ");
            }

            args[0]; /* origem */
            args[1]; /* destino */
        }
        /* PRECISA REALIZAR FUNCAO */
        else if(!strcmp(buf_break, "mkdir"))
        {
            char * dirname = strtok(NULL, " ");
            
            /* if(dirname == NULL) 
                printf("Precisa de mais argumentos.\n");

            else if(!mkdir(dirname,0777)) 
                printf("Criado o diretório de nome %s.\n", dirname);

            else 
                printf("Não foi possível criar o diretório.\n"); */
        }
        /* PRECISA REALIZAR FUNCAO */
        else if(!strcmp(buf_break, "rmdir"))
        {
            char * dirname = strtok(NULL, " ");
            
            /* if(dirname == NULL) 
                printf("Precisa de mais argumentos.\n");

            else if(!mkdir(dirname,0777)) 
                printf("Criado o diretório de nome %s.\n", dirname);

            else 
                printf("Não foi possível criar o diretório.\n"); */
        }
        /* PRECISA REALIZAR FUNCAO */
        else if(!strcmp(buf_break, "cat"))
        {
            char * dirname = strtok(NULL, " ");
            
            /* if(dirname == NULL) 
                printf("Precisa de mais argumentos.\n");

            else if(!mkdir(dirname,0777)) 
                printf("Criado o diretório de nome %s.\n", dirname);

            else 
                printf("Não foi possível criar o diretório.\n"); */
        }
        /* PRECISA REALIZAR FUNCAO */
        else if(!strcmp(buf_break, "touch"))
        {
            char * dirname = strtok(NULL, " ");
            
            /* if(dirname == NULL) 
                printf("Precisa de mais argumentos.\n");

            else if(!mkdir(dirname,0777)) 
                printf("Criado o diretório de nome %s.\n", dirname);

            else 
                printf("Não foi possível criar o diretório.\n"); */
        }
        /* PRECISA REALIZAR FUNCAO */
        else if(!strcmp(buf_break, "rm"))
        {
            char * dirname = strtok(NULL, " ");
            
            /* if(dirname == NULL) 
                printf("Precisa de mais argumentos.\n");

            else if(!mkdir(dirname,0777)) 
                printf("Criado o diretório de nome %s.\n", dirname);

            else 
                printf("Não foi possível criar o diretório.\n"); */
        }
        /* PRECISA REALIZAR FUNCAO */
        else if(!strcmp(buf_break, "ls"))
        {
            char * dirname = strtok(NULL, " ");
            
            /* if(dirname == NULL) 
                printf("Precisa de mais argumentos.\n");

            else if(!mkdir(dirname,0777)) 
                printf("Criado o diretório de nome %s.\n", dirname);

            else 
                printf("Não foi possível criar o diretório.\n"); */
        }
        /* PRECISA REALIZAR FUNCAO */
        else if(!strcmp(buf_break, "df"))
        {
            char * dirname = strtok(NULL, " ");
            
            /* if(dirname == NULL) 
                printf("Precisa de mais argumentos.\n");

            else if(!mkdir(dirname,0777)) 
                printf("Criado o diretório de nome %s.\n", dirname);

            else 
                printf("Não foi possível criar o diretório.\n"); */
        }
        /* PRECISA REALIZAR FUNCAO */
        else if(!strcmp(buf_break, "find"))
        {
            int i;

            for(i = 0; buf_break != NULL && i < 2; i++) { 
                args[i] = buf_break;
                buf_break = strtok(NULL, " ");
            }

            args[0]; /* diretorio */
            args[1]; /* arquivo */
        }

        else if(!strcmp(buf_break, "sai"))
        {
            /* freeProgram(); */
            exit(1);
        }
        /* Caso não reconheça nenhum comando */
        else
        {
            printf("Comando não identificado.\n");
            args[0] = NULL;
        }


       free(buffer);
    }

    free(prompt);
    /* free(buf_break); */

    return 0;
}

char * definePrompt()
{
    char * prompt;
    int prompt_size;
    
    prompt_size = 7; 

    prompt = (char *) malloc(prompt_size * sizeof(char*));
    strcpy(prompt, "[ep3]: ");


    return prompt;
}