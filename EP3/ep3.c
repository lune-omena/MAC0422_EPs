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
Bloco  FAT[BLOCOS];
void * admin[BLOCOS];


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

    for(int i = 0; i < BLOCOS; i++)
        bitmap[i] = 1; // blocos livres representados por 1

    // indica próximo espaço vago?
    int atual_bitmap = 51;

    for(int i = 0; i < atual_bitmap; i++)
        bitmap[i] = 1; // blocos ocupados pelo bitmap e pelo FAT
    
    /*
    for(int i = 0; i < BLOCOS; i++)
        FAT[i] = NULL;*/

    /* testando posição de admin */
    printf("Posicao 0 do bitmap pelo bitmap: %d\n",  bitmap[0]);
    printf("Posicao 1 do bitmap pelo bitmap: %d\n",  bitmap[1]);
    int * hello = (int *) admin[0];
    int posicao = 1;
    printf("Posicao 0 do bitmap pelo vetorzao: %d\n",  *(int *) &admin[0][sizeof(int)*0]);
    printf("Posicao 1 do bitmap pelo vetorzao: %d\n",  *(int *) &admin[0][sizeof(int)*posicao]);

    /* inicialização do FAT */
    FAT[0].prox = -1;
    FAT[0].endereco = admin[0];

    for(int i = 1; i < atual_bitmap; i++)
        FAT[i].prox =  i+1;
    
    FAT[atual_bitmap-1].prox = -1;
    FAT[1].endereco = admin[1];
    

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

                    //diretório como lista com 1 entrada para cada arquivo
                    Diretorio * raiz;

                    raiz->arqv = NULL;
                    raiz->arqv_prox = NULL;
                    raiz->dir_filho = NULL;
                    raiz->dir_prox = NULL; // o dir_prox de raiz deve sempre ser NULL!!!
                    raiz->pos_fat = atual_bitmap;
                    raiz->t_criado = raiz->t_alterado = raiz->t_acesso = (unsigned) time(NULL);

                    //Vetor admin -> preciso incluir algo como o / no bloco?
                    admin[atual_bitmap] = (void *) raiz;

                    //FAT
                    FAT[atual_bitmap].prox = -1;
                    FAT[atual_bitmap].endereco = admin[atual_bitmap];

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

            char texto[4000];
            char aux[4000];
            char aux2[4000] = "\0";
            int tam_txt = 0;
            int tam_kbytes = 0;
            int ini = -1; 

            if(!access(args[1], F_OK)) { // encontrou path para dest e orig
                FILE * f_cp = fopen(args[0], "r");

                if(f_cp) { // encontrou path para arquivo texto

                    // encontra espaço vago do fat e do bitmap
                    // guarda posição atual caso necessite "fechar" em -1 no fat
                    // se for \n, checa se aux tá cheio, se não tiver adiciona o resto no aux e fecha se >= 4000
                    // se não checa se o tamanho da string é <= 4000, se for então ele ocupou o espaço total e aloca
                    //              se não for, quer dizer que acabou definitivamente e ele vai receber posição -1


                    Arquivo * novo_arquivo = (Arquivo *) malloc(sizeof(Arquivo));
                    // preciso atualizar primeiro na FAT e pegar tamanho
                    int pos_ultimo = -1;

                    //  to read N characters, the length specification must be specified as N+1.
                    while(fgets(texto, 4001, f_cp)) { 
                        
                        // preciso zerar o aux em algum momento
                        tam_txt = strlen(texto);
                        
                        // checando se último elemento era newline
                        if(texto[tam_txt-1] == "\n") { // falta ler mais coisa, talvez
                            //strlen não conta \0
                            if(strlen(aux) + tam_txt >= 4000) {

                                if(tam_txt > 4000 - strlen(aux)) { // se o tamanho da nova string for maior que o max
                                    // "Corta string"
                                    strncat(aux, texto, 4000 - strlen(aux));
                                    // aux carrega, agora, o conteúdo primeiro!
                                    strncpy(texto, texto + (4000 - strlen(aux)), strlen(texto)- (4000 - strlen(aux)));
                                    // texto carrega, agora, o resto

                                    // encontra espaço vago no bitmap/FAT
                                    int pos = find_bitmap();

                                    admin[pos] = (void *) aux;
                                    strcpy(aux, texto);
                                    
                                    // insere no bitmap/FAT
                                    bitmap[pos] = 0;
                                    pos_ultimo = pos;

                                    //FAT encontra espaço vaGO MUDAR DPS
                                    if(pos_ultimo != -1)
                                        FAT[pos_ultimo].prox = pos;
                                    else
                                        ini = pos;
                                    

                                    FAT[pos].prox = -1;
                                    FAT[pos].endereco = admin[pos];
                                    atual_bitmap++;

                                    tam_kbytes += 4;

                                }
                                else { // conteúdo  == 
                                    //  Concatena em aux
                                    strcat(aux, texto);

                                    // encontra espaço vago no bitmap/FAT
                                    int pos = find_bitmap();

                                    // insere no bitmap/FAT

                                    admin[pos] = (void *) aux;
                                    strcpy(aux, aux2);
                                    
                                    // insere no bitmap/FAT
                                    bitmap[pos] = 0;
                                    pos_ultimo = pos;

                                    //FAT encontra espaço vaGO MUDAR DPS
                                    if(pos_ultimo != -1)
                                        FAT[pos_ultimo].prox = pos;
                                    else
                                        ini = pos;

                                    FAT[pos].prox = -1;
                                    FAT[pos].endereco = admin[pos];
                                    atual_bitmap++;

                                    tam_kbytes += 4;
                                }

                            }
                            else 
                                strcat(aux, texto);
                            
                        }
                        else if(strlen(aux) + tam_txt > 4000) { // pode estar faltando mais coisa
                            // "Corta string"
                            strncat(aux, texto, 4000 - strlen(aux));
                            // aux carrega, agora, o conteúdo primeiro!
                            strncpy(texto, texto + (4000 - strlen(aux)), strlen(texto)- (4000 - strlen(aux)));
                            // texto carrega, agora, o resto

                            // encontra espaço vago no bitmap/FAT
                            int pos = find_bitmap();

                            admin[pos] = (void *) aux;
                            strcpy(aux, texto);
                            
                            // insere no bitmap/FAT
                            bitmap[pos] = 0;
                            pos_ultimo = pos;

                            //FAT encontra espaço vaGO MUDAR DPS
                            if(pos_ultimo != -1)
                                FAT[pos_ultimo].prox = pos;
                            else
                                ini = pos;

                            FAT[pos].prox = -1;
                            FAT[pos].endereco = admin[pos];
                            atual_bitmap++;

                            tam_kbytes += 4;
                        }
                        else if (strlen(aux) + tam_txt == 4000) {
                            // conteúdo  == 
                            //  Concatena em aux
                            strcat(aux, texto);

                            // encontra espaço vago no bitmap/FAT
                            int pos = find_bitmap();

                            // insere no bitmap/FAT

                            admin[pos] = (void *) aux;
                            strcpy(aux, aux2);
                            
                            // insere no bitmap/FAT
                            bitmap[pos] = 0;
                            pos_ultimo = pos;

                            //FAT encontra espaço vaGO MUDAR DPS
                            if(pos_ultimo != -1)
                                FAT[pos_ultimo].prox = pos;
                            else
                                ini = pos;

                            FAT[pos].prox = -1;
                            FAT[pos].endereco = admin[pos];
                            atual_bitmap++;

                            tam_kbytes +=4;
                        }
                        else { // terminou o file
                            strcat(aux, texto);

                            int pos = find_bitmap();

                            admin[pos] = (void *) aux;
                            
                            // insere no bitmap/FAT
                            bitmap[pos] = 0;
                            pos_ultimo = pos;

                            //FAT encontra espaço vaGO MUDAR DPS
                            if(pos_ultimo != -1)
                                FAT[pos_ultimo].prox = pos;
                            else
                                ini = pos;

                            FAT[pos].prox = -1;
                            FAT[pos].endereco = admin[pos];
                            atual_bitmap++;

                            tam_kbytes += (strlen(aux))/1000;
                        }
                        
                    }

                    // atualizar dados do arquivo dentro do diretório que está
                    novo_arquivo->arq_acesso = novo_arquivo->arq_alterado = novo_arquivo->arq_criado = (unsigned) time(NULL);
                    novo_arquivo->tamanho = (tam_kbytes)*1000;
                    novo_arquivo->pos_fat = ini;

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

int find_bitmap() {
    int aux;

    for(aux = 0; aux < 24414 && bitmap[aux]; aux++);

    if(aux == 24414)
        return -1;
    
    return aux;
}