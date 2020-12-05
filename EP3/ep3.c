/* EP3 - Sistemas Operacionais
 * 
 * Nome: Lara Ayumi Nagamatsu               NUSP: 9910568
 * Nome: Lucy Anne de Omena Evangelista     NUSP: 11221776
 * 
 */

/* Bibliotecas */
#include "ep3.h"                 /* header */
#include <stdio.h>               /* printf(), fgets()... */
#include <stdlib.h>              /* malloc(), free() */

#include <readline/readline.h>   /* ler linha de comando */
#include <readline/history.h>    /* historico do terminal */
#include <string.h>              /* strcmp(), strtok()... */

#include <unistd.h>              /* access() -> checa path se existe */
#include <time.h>                /* pegar timestamp */

#define BLOCOS 24414             /* blocos totais */


int main ()
{
    /* auxiliares para o terminal */
    char * buffer;                                
    char * prompt; 
    char * buf_break;
    // 24414 posições vem de 100MB/4*1024, aula 15
    int bitmap[BLOCOS]; 
    /* O FAT é utilizado para armazenamento de arquivos */
    Bloco * FAT[BLOCOS];
    void  * admin[BLOCOS];
    
    
    admin[0] = (void *) bitmap;
    admin[1] = (void *) FAT;
    /* argumentos usados como parâmetros para as funcoes cp e find */
    char * args[2];  
    
    /* FORMATAÇÃO DO PROMPT */
    prompt = definePrompt();

    /* O bitmap representa os blocos vazios e ocupados, como existem 100MB de espaço total, sendo 4KB para cada
     * bloco, temos 100000KB/4*1024 = 24414 blocos de espaço!!!*/

    /* Por definição, bitmap guarda 1 bit por bloco! Portanto, como temos 24414 espaços do bitmap, são no total
     * 24414 bits ocupados que cabem em 32K bits (4KB), portanto o bitmap ocupa 1 bloco */
    /* Por definição, FAT guarda 4 bytes por entrada! Como são 100MB de dados, com 4KB para cada bloco,
     * somam no total 25000 entradas na tabela. Mas o FAT utiliza 4 bytes por entrada, portanto o espaço
     * total ocupado pelo FAT é dado por 100KB. Logo, ocupa 25 blocos. */
    /* A FAT vai ocupar o dobro porque vamos utilizá-la para endereçar diretamente os blocos */
    /* No total, são 51 espaços pré-ocupados */

    

    for(int i = 0; i < BLOCOS; i++)
        bitmap[i] = 1; // blocos livres representados por 1

    // indica próximo espaço vago?
    int atual_bitmap = 51;

    for(int i = 0; i < atual_bitmap; i++)
        bitmap[i] = i; // blocos ocupados pelo bitmap e pelo FAT

    /* testando posição de admin */
    printf("Posicao 0 do bitmap pelo bitmap: %d\n",  bitmap[0]);
    printf("Posicao 1 do bitmap pelo bitmap: %d\n",  bitmap[1]);
    int * hello = (int *) admin[0];
    int posicao = 1;
    printf("Posicao 0 do bitmap pelo vetorzao: %d\n",  *(int *) &admin[0][sizeof(int)*0]);
    printf("Posicao 1 do bitmap pelo vetorzao: %d\n",  *(int *) &admin[0][sizeof(int)*posicao]);
    


    for(int i = 0; i < BLOCOS; i++)
        FAT[i] = NULL;

    // O PRIMEIRO BLOCO DA FAT FICA NO DIRETÓRIO
    // não sei exatamente o que eu quis dizer com isso, mas acho que era para dizer que
    // para cada diretório nas listinhas, a indicação na tabela fat do conteúdo é dado lá

    /* Quando eu encher o sistema de arquivos na hora de corrigir o seu EP, eu vou desmontar o arquivo, 
     * fechar o EP e dar um ls no arquivo que foi criado. */

    FILE *fp;

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
            char * arquivo = strtok(NULL, "\n"); // delimitador é o fim do texto, ou seja \n
            
            if (arquivo != NULL)
            printf("%s\n", arquivo);
            
            if(arquivo == NULL) 
                printf("Precisa de mais argumentos.\n");

            else if(!access(arquivo, F_OK)) { // checa se path existe
                
                /* Cria path para o arquivo.txt */
                int tamanho = strlen(arquivo);
                char new_path[tamanho+12]; 

                for (int i = 0; i < tamanho+12; i++)
                    new_path[i] = NULL;

                strcat(new_path, arquivo);
                strcat(new_path, "/arquivo.txt");

                /* Checa se já existe arquivo.txt */
                fp = fopen(new_path, "r+");

                if(fp != NULL) {
                    printf("Sistema de arquivos recuperado\n");
                    // precisamos carregar o sistema de arquivos com o fat, bitmap, etc.
                }
                else {
                    printf("Temos que criar um novo sistema de arquivos\n");
                    fp = fopen(new_path, "w"); // só vamos mexer com o arquivo final no umount, certo?
                    
                    /* aqui criamos o "/"? */

                    //bitmap
                    bitmap[atual_bitmap] = 0;
                    atual_bitmap++;

                    

                    //diretório como lista com 1 entrada para cada arquivo
                    Diretorio raiz;
                    raiz.prox = NULL;
                    raiz.arqv = NULL;
                    raiz.t_criado = raiz.t_alterado = raiz.t_acesso = (unsigned) time(NULL);

                }
            }
            else 
                printf("Esse path não existe\n");
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