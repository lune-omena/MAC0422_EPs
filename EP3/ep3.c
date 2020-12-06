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


/* VARIÁVEIS GLOBAIS */
int    bitmap[BLOCOS]; 
Bloco *FAT[BLOCOS];
void * admin[BLOCOS];

int registraAdmin(char * arquivo);
int recebeAdmin(char * arquivo);

int main ()
{
    /* auxiliares para o terminal */
    char * buffer;                                
    char * prompt; 
    char * buf_break;

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

    for (int i = 0; i < BLOCOS; i ++)
        FAT[i] = malloc(sizeof(Bloco));

    for(int i = 0; i < BLOCOS; i++)
        bitmap[i] = 1; // blocos livres representados por 1

    // indica próximo espaço vago?
    int atual_bitmap = 51;

    for(int i = 0; i < atual_bitmap; i++)
        bitmap[i] = 0; // blocos ocupados pelo bitmap e pelo FAT
    
    /*
    for(int i = 0; i < BLOCOS; i++)
        FAT[i] = NULL;*/

    /* testando posição de admin */
    /* printf("Posicao 0 do bitmap pelo bitmap: %d\n",  bitmap[0]);
    printf("Posicao 1 do bitmap pelo bitmap: %d\n",  bitmap[1]);
    int * hello = (int *) admin[0];
    int posicao = 1;
    printf("Posicao 0 do bitmap pelo vetorzao: %d\n",  *(int *) &admin[0][sizeof(int)*0]);
    printf("Posicao 1 do bitmap pelo vetorzao: %d\n",  *(int *) &admin[0][sizeof(int)*posicao]); */

    /* inicialização do FAT */
    FAT[0]->prox = -1;
    FAT[0]->endereco = admin[0];

    for(int i = 1; i < atual_bitmap; i++)
        FAT[i]->prox =  i+1;
    for(int i = atual_bitmap; i < BLOCOS; i++)
        FAT[i]->prox =  -1;
    
    FAT[atual_bitmap-1]->prox = -1;
    FAT[1]->endereco = admin[1];

    // Diretório "/" é o raiz
    Diretorio * raiz; // criei antes
    raiz = (Diretorio *) malloc(sizeof(Diretorio));

    // O PRIMEIRO BLOCO DA FAT FICA NO DIRETÓRIO
    // não sei exatamente o que eu quis dizer com isso, mas acho que era para dizer que
    // para cada diretório nas listinhas, a indicação na tabela fat do conteúdo é dado lá

    /* Quando eu encher o sistema de arquivos na hora de corrigir o seu EP, eu vou desmontar o arquivo, 
     * fechar o EP e dar um ls no arquivo que foi criado. */

    FILE *fp;

    using_history();
    printf("Digite CTRL+D para finalizar.\n");
    char * caminho2 = "/media/lune/Data/Desktop/USP/Github/MAC0422_EPs/EP3/arquivo.txt";
    char * caminho = "/media/lune/Data/Desktop/USP/Github/MAC0422_EPs/EP3";

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
            //else if(!access(caminho, F_OK)) { // checa se path existe
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
                //fp = fopen(caminho2, "r+");

                if(fp != NULL) {
                    printf("Sistema de arquivos recuperado\n");
                    // precisamos carregar o sistema de arquivos com o fat, bitmap, etc.
                    puts(new_path);
                    fclose(fp);
                    // precisamos carregar o sistema de arquivos com o fat, bitmap, etc.
                    //recebeAdmin(caminho2);
                    recebeAdmin(new_path);
                }
                else {
                    printf("Temos que criar um novo sistema de arquivos\n");
                    fp = fopen(new_path, "w"); // só vamos mexer com o arquivo final no umount, certo?
                    //fp = fopen(caminho2, "w"); // só vamos mexer com o arquivo final no umount, certo?
                    /* aqui criamos o "/"? */
                    //diretório como lista com 1 entrada para cada arquivo

                    raiz->nome = (char *) malloc(strlen("/")+1);
                    strcpy(raiz->nome, "/");
                    raiz->arqv = NULL;
                    raiz->arqv_prox = NULL;
                    raiz->dir_filho = NULL;
                    raiz->dir_prox = NULL; // o dir_prox de raiz deve sempre ser NULL!!!
                    raiz->t_criado = raiz->t_alterado = raiz->t_acesso = (unsigned) time(NULL);
                    
                    raiz->pos_fat = atual_bitmap;

                    //Vetor admin -> preciso incluir algo como o / no bloco?
                    admin[atual_bitmap] = (void *) raiz;

                    //FAT
                    FAT[atual_bitmap]->prox = -1;
                    FAT[atual_bitmap]->endereco = admin[atual_bitmap];

                    //bitmap
                    bitmap[atual_bitmap] = 0;
                    atual_bitmap++;

                }
            }
            else 
                printf("Esse path não existe\n");
        }
        else if(!strcmp(buf_break, "umount"))
        {
            //char * dirname = strtok(NULL, " ");
            
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
            buf_break = strtok(NULL, " "); // pega cp

            for(i = 0; buf_break != NULL && i < 2; i++) { // origem e destino
                args[i] = buf_break;
                buf_break = strtok(NULL, " ");
            }

            //args[0]; /* origem */
            //args[1]; /* destino */
            char texto[4000];
            char aux[4000];
            char aux2[4000] = "\0";
            int tam_txt = 0;
            int tam_bytes = 0;
            int ini = -1; 

            Diretorio * dir = find_dir(args[1], raiz);

            //if(!access(args[1], F_OK)) { // encontrou path para dest e orig
            if(dir) {

                FILE * f_cp = fopen(args[0], "r");
                char * c_pointer = (char *) malloc(sizeof(char));

                if(f_cp) { // encontrou path para arquivo texto

                    printf("Encontrou arquivo! O diretório destino é |%s|!\n", dir->nome);

                    Arquivo * novo_arquivo = (Arquivo *) malloc(sizeof(Arquivo));
                    // preciso atualizar primeiro na FAT e pegar tamanho
                    int pos_ultimo = -1;

                    char c = fgetc(f_cp);


                    while(1) {

                        if( c == EOF || strlen(aux) + 1 == 4000 ) {

                            if(c == EOF)
                                strcpy(c_pointer, aux2);
                            else
                                *c_pointer = c;
                            
                            strcat(aux, c_pointer);

                            int pos = find_bitmap();

                            admin[pos] = (void *) aux;
                            //printf("aqui: |%s|\n", aux);
                            
                            // insere no bitmap/FAT
                            bitmap[pos] = 0;
                            pos_ultimo = pos;

                            //FAT encontra espaço vaGO MUDAR DPS
                            if(pos_ultimo != -1)
                                FAT[pos_ultimo]->prox = pos;
                            else
                                ini = pos;

                            FAT[pos]->prox = -1;
                            FAT[pos]->endereco = admin[pos];
                            atual_bitmap++;

                            tam_bytes += strlen(aux);
                            strcpy(aux, aux2);

                            if(c == EOF)
                                break;

                        }
                        else {
                            *c_pointer = c;
                            strcat(aux, c_pointer);
                        }
                        
                        c = fgetc(f_cp);

                    }

                    // atualizar dados do arquivo dentro do diretório que está
                    novo_arquivo->arq_acesso = novo_arquivo->arq_alterado = novo_arquivo->arq_criado = (unsigned) time(NULL);
                    novo_arquivo->tamanho = (tam_bytes);
                    novo_arquivo->pos_fat = ini;
                    novo_arquivo->nome = nome_arquivo(args[0]);
                    printf("%s criado\n", novo_arquivo->nome);

                    // INSERIR NO diretório

                }
                else 
                    printf("Não existe um arquivo (origem) com esse nome.\n");
            }
            else
                printf("Não existe path destino com esse nome.\n");
            

        }
        /* PRECISA REALIZAR FUNCAO */
        else if(!strcmp(buf_break, "mkdir"))
        {
            //char * dirname = strtok(NULL, " ");
            
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
            //char * dirname = strtok(NULL, " ");
            
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
            //char * dirname = strtok(NULL, " ");
            
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
            //char * dirname = strtok(NULL, " ");
            
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
            //char * dirname = strtok(NULL, " ");
            
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
            //char * dirname = strtok(NULL, " ");
            
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
            printf("\nEntrei no sai");

            registraAdmin(caminho2);
            //registraAdmin(new_path);

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


int registraAdmin(char * arquivo)
{
    FILE *fp;
    int i;

    fp = fopen(arquivo, "w");

    if (fp != NULL)
    {
        printf("\nAbri o arquivo");
    
        for (i = 0; i < BLOCOS; i++)
            fprintf(fp, "%d", bitmap[i]);
        fputc('\n', fp);
        for (i = 0; i < BLOCOS; i++)
            fprintf(fp, "%5d", FAT[i]->prox);
        
        printf("\nGRAVAMOS %d BLOCOS\n", i);
        printf("\nSai!");
    }
    
    //fseek( fp, 0, SEEK_CUR );
    
    fclose(fp);
    return 1;
}

int recebeAdmin(char * arquivo)
{
    FILE *fp;
    int i = 0;
    char ch;
    char * textfat[5];


    fp = fopen(arquivo, "r");
    
    if (fp != NULL)
    {
        /* First line: Bitmap */
        while ((ch = fgetc(fp)) != '\n' && ch != EOF)
        {    
            bitmap[i] = atoi(&ch);
            i++;
        }

        /* Second line: FAT */
        i = 0;
        while ((ch = fgets(textfat, 6, fp)) != NULL && ch != EOF)
        {    
            /* printf("\nValor fat original 1 : %p - contador: %d ué value: %d", fat[i], i, fat[24413]->prox);
            printf(" Valor arquivo: %d", atoi(textfat)); */
            FAT[i]->prox = atoi((const char *)textfat);
            i++;
            if (i == BLOCOS)
                break;
        }
    }
    

    printf("\nSai!");
    fclose(fp);
    return 1;
}

int find_bitmap() {
    int aux;

    for(aux = 0; aux < 24414 && bitmap[aux]; aux++);

    if(aux == 24414)
        return -1;
    
    return aux;
}

Diretorio * find_dir(char * nome, Diretorio * raiz) {
    Diretorio * aux = raiz;

    if(!strcmp(nome, raiz->nome)) // é o "/"
        return raiz;

    char * token = strtok(nome, "/");

    while(token) {
        while(aux && !strcmp(token, aux->nome))
            aux = aux->dir_prox;
        
        if(aux)
            token = strtok(nome, "/");
        else token = NULL;
    }

    return aux;
}

char * nome_arquivo(char * path) {  // Devolve nome do arquivo dado por um path

    char * token = strtok(path, "/");
    char * past;

    while(token) {
        past = token;
        token = strtok(NULL, "/");
    }

    return past;
}