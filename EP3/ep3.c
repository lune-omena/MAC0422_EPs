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
int     bitmap[BLOCOS]; 
Bloco * FAT[BLOCOS];
void  * admin[BLOCOS];

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

    /* O bitmap guarda 4 bytes por bloco! Portanto, como temos 24414 espaços do bitmap, são no total
     * 24414*4 bytes = ~97K ocupados que cabem em 7 blocos */
    /* Por definição, FAT guarda 4 bytes por entrada! Como são 100MB de dados, com 4KB para cada bloco,
     * somam no total 24414 entradas na tabela. Mas o FAT utiliza 8 bytes por entrada, portanto o espaço
     * total ocupado pelo FAT é dado por ~195KB. Logo, ocupa 49 blocos. */
    /* No total, são 49+7 = 56 espaços pré-ocupados */ 

    for (int i = 0; i < BLOCOS; i ++)
        FAT[i] = malloc(sizeof(Bloco));

    for(int i = 0; i < BLOCOS; i++)
        bitmap[i] = 1; // blocos livres representados por 1

    // indica próximo espaço vago?
    int atual_bitmap = 56;
    int inicio_dados = atual_bitmap; /* recebe inicio do sistema de dados*/
    

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
    /*
    Diretorio * raiz; // criei antes
    raiz = (Diretorio *) malloc(sizeof(Diretorio));*/
    Celula * raiz; // criei antes
    raiz = (Celula *) malloc(sizeof(Celula));


    // O PRIMEIRO BLOCO DA FAT FICA NO DIRETÓRIO
    // não sei exatamente o que eu quis dizer com isso, mas acho que era para dizer que
    // para cada diretório nas listinhas, a indicação na tabela fat do conteúdo é dado lá

    /* Quando eu encher o sistema de arquivos na hora de corrigir o seu EP, eu vou desmontar o arquivo, 
     * fechar o EP e dar um ls no arquivo que foi criado. */

    FILE *fp;

    using_history();
    printf("Digite CTRL+D para finalizar.\n");
    /* char * caminho2 = "/home/lara/code/4o/so/MAC0422_EPs/EP3/arquivo.txt";
    char * caminho = "/home/lara/code/4o/so/MAC0422_EPs/EP3"; */
    char * caminho2 = "/media/lune/Data/Desktop/USP/Github/MAC0422_EPs/EP3/arquivo.txt";
    char * caminho = "/media/lune/Data/Desktop/USP/Github/MAC0422_EPs/EP3";
    char * arquivo;
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
            arquivo = strtok(NULL, "\n"); // delimitador é o fim do texto, ou seja \n
            
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
                    raiz->tipo = 'D';
                    raiz->node_filho = raiz->node_prox = NULL; // o node_prox de raiz deve sempre ser NULL!!!
                    raiz->filhos = 0;
                    raiz->t_criado = raiz->t_alterado = raiz->t_acesso = (unsigned) time(NULL);
                    raiz->pos_fat = atual_bitmap;
                    raiz->tamanho = 0;

                    printf("%d", raiz->t_acesso);

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
            char aux[4000] = "";
            char aux2[4000] = "";
            int tam_txt = 0;
            int tam_bytes = 0;
            int ini = -1; 

            //Diretorio * dir = find_dir(args[1], raiz);
            Celula * dir = find_dir(args[1], raiz);

            //if(!access(args[1], F_OK)) { // encontrou path para dest e orig
            if(dir) {

                FILE * f_cp = fopen(args[0], "r");
                char * c_pointer = (char *) malloc(sizeof(char));

                if(f_cp) { // encontrou path para arquivo texto

                    printf("Encontrou arquivo! O diretório destino é |%s|!\n", dir->nome);

                    if(dir->filhos == 10) {
                        printf("O diretório chegou ao seu número limite de arquivos(10)!\n");
                        printf("Não é possível adicionar este arquivo no destino.\n");
                    }
                    else {
                        //Arquivo * novo_arquivo = (Arquivo *) malloc(sizeof(Arquivo));
                        Celula * novo_arquivo = (Celula *) malloc(sizeof(Celula));
                        // preciso atualizar primeiro na FAT e pegar tamanho
                        int pos_ultimo = -1;

                        char c = fgetc(f_cp);

                        while(1) {

                            if( c == EOF || strlen(aux) + 1 == 4000 ) {

                                if(c == EOF)
                                    strcpy(c_pointer, aux2);
                                else {
                                    *c_pointer = c;
                                    //printf("%d é o tamanho de c_pointer\n", strlen(c_pointer));
                                }
                                
                                strcat(aux, c_pointer);

                                int pos = find_bitmap();
                                //printf("%d\n", pos);

                                admin[pos] = (void *) aux;
                                printf("aqui: |%s|\n", aux);

                                //FAT encontra espaço vaGO MUDAR DPS
                                if(pos_ultimo != -1)
                                    FAT[pos_ultimo]->prox = pos;
                                else
                                    ini = pos;

                                // insere no bitmap/FAT
                                bitmap[pos] = 0;
                                pos_ultimo = pos;

                                FAT[pos]->prox = -1;
                                FAT[pos]->endereco = admin[pos];
                                atual_bitmap++;

                                tam_bytes += strlen(aux);
                                strcpy(aux, aux2);

                                if(c == EOF)
                                    break;

                            }
                            else {
                                c_pointer = (char *) malloc(sizeof(char));
                                *c_pointer = c;
                                strcat(aux, c_pointer);
                            }
                            
                            c = fgetc(f_cp);

                        }

                        // atualizar dados do arquivo dentro do diretório que está
                        novo_arquivo->t_acesso = novo_arquivo->t_alterado = novo_arquivo->t_criado = (unsigned) time(NULL);
                        printf("%d", novo_arquivo->t_acesso);
                        novo_arquivo->tamanho = (tam_bytes);
                        novo_arquivo->pos_fat = ini;
                        novo_arquivo->node_filho = (Celula *) malloc(sizeof(Celula));
                        novo_arquivo->node_prox = (Celula *) malloc(sizeof(Celula));
                        novo_arquivo->node_prox = novo_arquivo->node_filho  = NULL;
                        novo_arquivo->filhos = 0;
                        novo_arquivo->tipo = 'A';
                        novo_arquivo->nome = nome_arquivo(args[0]);

                        printf("%s criado\n", novo_arquivo->nome);

                        // INSERIR NO diretório
                        if(dir->filhos == 0)
                            dir->node_filho = novo_arquivo;
                        else
                            devolve_ult(dir)->node_prox = novo_arquivo;
                        
                        dir->filhos++;

                    }

                }
                else 
                    printf("Não existe um arquivo (origem) com esse nome.\n");
            }
            else
                printf("Não existe path destino com esse nome.\n");

        }
        else if(!strcmp(buf_break, "mkdir"))
        { // /home/lara/code/4o/so/MAC0422_EPs/EP3

            // Necessário fazer cópia de dirname para não alterar seu conteúdo!
            char * dirname = strtok(NULL, " ");
            char * aux_dir = (char *) malloc((strlen(dirname)+1)*sizeof(char));
            printf("%s, com tamanho %d \n", dirname, strlen(dirname));
            strcpy(aux_dir, dirname);

            // Separar o nome do diretório entre parte existente e parte criada
            printf("Criando diretório!\n");

            char * novo = nome_arquivo(aux_dir); // pega nome do diretório SEM /
            char * aux_novo;
            int nome_size = strlen(novo);
            //printf("%d é o tamanho do novo diretório\n", nome_size);

            if(nome_size) { // path existe!
                Celula * new = (Celula *) malloc(sizeof(Celula));
                Celula * aux = NULL, * ant = NULL;
                int n_pai = strlen(dirname) - strlen(novo) - 1; //  retira o / do fim do nome
                //if(dirname[strlen(dirname)] == '/')
                //    n_pai--;

                printf("%d é o tamanho do diretório pai!\n DIRNAME: %s| AUX_DIR: %s\n", n_pai, dirname, aux_dir);
                char * pai = (char *) malloc((n_pai+1)*sizeof(char)); // +1 para o \0

                strncpy(pai, dirname, n_pai); 

                printf("%s é o nome do novo diretório (%d), %s é seu pai (%d)!\n", novo, strlen(novo), pai, strlen(pai));

                /*
                if(n_pai > 1) { // preciso pegar o pai, se não for "/"
                    aux_novo = (char *) malloc(strlen(pai)*sizeof(char));
                    strcpy(aux_novo, pai);
                    pai = nome_arquivo(aux_novo);
                    printf("\n\n%s (%d) %s (%d) %s (%d)\n\n", dirname, strlen(dirname), pai, strlen(pai), aux_novo, strlen(aux_novo));

                }*/
                
                aux = find_dir(pai, raiz);

                printf("%s é o nome do novo diretório (%d), %s é seu pai (%d)!\n", novo, strlen(novo), aux->nome, strlen(aux->nome));


                if(!aux) {
                    printf("Esse path não existe... Indique um path possível\n");
                }
                else if(aux->filhos == 10) {
                    printf("O diretório %s chegou no limite de arquivos (10)!\n", aux->nome);
                }
                else {
                    
                    if(aux->filhos)
                        devolve_ult(aux)->node_prox = new;
                    else
                        aux->node_filho = new;
                    
                    
                    aux->filhos += 1;
    
                    new->t_acesso = new->t_alterado = new->t_criado = (unsigned) time(NULL);
                    printf("Tempo: |%d|\n", new->t_acesso);
                    new->nome = novo;
                    printf("Nome: |%s|\n", new->nome);
                    new->filhos = 0;
                    printf("Filhos: |%d|\n", new->filhos);
                    new->tipo = 'D';
                    printf("Tipo: |%c|\n", new->tipo);
                    new->node_filho = new->node_prox = NULL;
                    new->tamanho = 0;
                    new->pos_fat = find_bitmap();
                    printf("Posição no bitmap/FAT: |%d|\n", new->pos_fat);
                    bitmap[new->pos_fat] = 0;
                    admin[new->pos_fat] = (void *) new;

                    printf("Agora %s é filho oficia de %s\n\n", new->nome, aux->nome);
                }

            }
        }
        /* PRECISA REALIZAR FUNCAO */
        else if(!strcmp(buf_break, "rmdir"))
        {
            char * dirname = strtok(NULL, " ");
            Celula * dir_node, * aux, * ant;

            // Copia dirname para um dummy para não alterar valor original
            char * org_aux = (char *) malloc((strlen(dirname)+1)*sizeof(char));
            strncpy(org_aux, dirname, strlen(dirname));
            int indice;

            // vai ter o path e o nome do arquivo
            // do tipo /home/lara/code/4o/so/MAC0422_EPs/EP3/oi.txt

            char * dir = nome_arquivo(org_aux);

            // Copia diretório a dir
            int dir_tam = strlen(dirname) - strlen(dir) - 1; // retira a "/"
            char * dir_ant = (char *) malloc((dir_tam+1)*sizeof(char));
            strncpy(dir_ant, dirname, dir_tam);
            printf("nome do path anterior ao diretório: %s\n", dir_ant);

            dir_node = find_dir(dir_ant, raiz);

            if(dir_node) { // dir_node deve ser pai do arquivo, logo:

                aux = dir_node->node_filho;
                int achou = 0;

                while(aux && !achou) {

                    if(aux->tipo == 'D'  && !strcmp(aux->nome, dir)) {
                        achou = 1;
                    }
                    else {
                        ant = aux;
                        aux = aux->node_prox;
                    }
                    
                }

                if(aux) { // encontrou o arquivo!
                    printf("Diretório encontrado :) %s\n", dirname);

                    // Primeiro vou deletar todos os arquivos que estiverem dentro do diretório:
                    Celula * arq = aux;

                    int resultado = remove_filhos(aux);

                    /*           
                    indice = aux->pos_fat;
                
                    if(ant && aux->node_prox) {
                        ant->node_prox = aux->node_prox;
                    }
                    else if(aux->node_prox) { // o aux era o primeiro
                        dir_node->node_filho = aux->node_prox;
                    }

                    indice = aux->pos_fat;
                    printf("\nPosicao na fat do arquivo a ser deletado: %d\n", indice);
                    int atual;
                    
                    while (FAT[indice]->prox != -1)
                    {
                        atual = indice;
                        indice = FAT[indice]->prox;
                        admin[atual] = NULL;

                        FAT[atual]->endereco = NULL;
                        FAT[atual]->prox = -1;
                        bitmap[atual] = 1;
                    }

                    admin[indice] = NULL;
                    FAT[indice]->endereco = NULL;
                    bitmap[indice] = 1;*/

                    printf("Arquivo foi deletado com sucesso!\n");
                }
                else 
                    printf("O arquivo não foi encontado.\n");

            }
            else
                printf("Esse path não existe\n");

        }
        /* PRECISA REALIZAR FUNCAO */
        else if(!strcmp(buf_break, "cat"))
        {
            char * dirname = strtok(NULL, " ");
            Celula * dir_node, * aux;

            // Copia dirname para um dummy para não alterar valor original
            char * org_aux = (char *) malloc((strlen(dirname)+1)*sizeof(char));
            strncpy(org_aux, dirname, strlen(dirname));
            int indice;

            // vai ter o path e o nome do arquivo
            // do tipo /home/lara/code/4o/so/MAC0422_EPs/EP3/oi.txt

            char * arqv = nome_arquivo(org_aux);

            // Copia diretório a dir
            int arq_tam = strlen(dirname) - strlen(arqv);
            char * dir = (char *) malloc((arq_tam+1)*sizeof(char));
            strncpy(dir, dirname, arq_tam);
            printf("nome do dir: %s\n", dir);

            dir_node = find_dir(dir, raiz);

            if(dir_node) { // dir_node deve ser pai do arquivo, logo:
                aux = dir_node->node_filho;
                int achou = 0;

                while(aux && !achou) {

                    if(aux->tipo == 'A'  && !strcmp(aux->nome, arqv)) {
                        achou = 1;
                    }
                    else
                        aux = aux->node_prox;
                    
                }

                if(aux) { // encontrou o arquivo!
                    printf("Arquivo encontrado :)\n");
                    
                    indice = aux->pos_fat;
                    int i = FAT[indice]->prox;
                    
                    if(aux->tamanho >= 4000) {
                        while(i != -1) {
                            printf("%s", (char *) admin[i]);
                            i = FAT[i]->prox;
                        }
                    }
                    else {
                        printf("%s\n", (char *) admin[indice]);
                    }
                }
                else 
                    printf("O arquivo não foi encontado.\n");

            }
            else
                printf("Esse path não existe\n");

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

            char * dirname = strtok(NULL, " ");
            Celula * dir_node, * aux, * ant;

            // Copia dirname para um dummy para não alterar valor original
            char * org_aux = (char *) malloc((strlen(dirname)+1)*sizeof(char));
            strncpy(org_aux, dirname, strlen(dirname));
            int indice;

            // vai ter o path e o nome do arquivo
            // do tipo /home/lara/code/4o/so/MAC0422_EPs/EP3/oi.txt

            char * arqv = nome_arquivo(org_aux);

            // Copia diretório a dir
            int arq_tam = strlen(dirname) - strlen(arqv);
            char * dir = (char *) malloc((arq_tam+1)*sizeof(char));
            strncpy(dir, dirname, arq_tam);
            printf("nome do dir: %s\n", dir);

            dir_node = find_dir(dir, raiz);

            if(dir_node) { // dir_node deve ser pai do arquivo, logo:

                aux = dir_node->node_filho;
                int achou = 0;

                while(aux && !achou) {

                    if(aux->tipo == 'A'  && !strcmp(aux->nome, arqv)) {
                        achou = 1;
                    }
                    else {
                        ant = aux;
                        aux = aux->node_prox;
                    }
                    
                }

                if(aux) { // encontrou o arquivo!
                    printf("Arquivo encontrado :)\n");
                    
                    indice = aux->pos_fat;
                
                    /* retira registro do diretorio anterior */
                    /* caso o anterior seja um irmão */
                    if(ant && aux->node_prox) {
                        ant->node_prox = aux->node_prox;
                    }
                    else if(aux->node_prox) { // o aux era o primeiro
                        dir_node->node_filho = aux->node_prox;
                    }
                    else {
                        dir_node->node_filho = NULL;
                    }

                    /* retira registro na FAT e bitmap */
                    indice = aux->pos_fat;
                    printf("\nPosicao na fat do arquivo a ser deletado: %d\n", indice);
                    int atual;
                    
                    while (FAT[indice]->prox != -1)
                    {
                        atual = indice;
                        indice = FAT[indice]->prox;
                        /* essa seria a parte de apagar a entrada em admin[x] */
                        /* free(FAT[atual]->endereco)*/
                        admin[atual] = NULL;

                        FAT[atual]->endereco = NULL;
                        FAT[atual]->prox = -1;
                        bitmap[atual] = 1;
                    }

                    admin[indice] = NULL;
                    FAT[indice]->endereco = NULL;
                    bitmap[indice] = 1;

                    printf("Arquivo deletado com sucesso!\n");
                }
                else 
                    printf("O arquivo não foi encontado.\n");

            }
            else
                printf("Esse path não existe\n");

            /*
            char * dirname = strtok(NULL, " ");
            int result;

            if(dirname == NULL)
            {
                printf("Precisa de mais argumentos.\n");
                break;
            }
            result = rm_arquivo(dirname, inicio_dados); 

            if (result)
                printf("\nArquivo apagado com sucesso.\n");
            else
                printf("\nArquivo não encontrado ou problema ao excluir o arquivo. Tente novamente.\n");
                */

        }
        /* PRECISA REALIZAR FUNCAO */
        else if(!strcmp(buf_break, "ls"))
        {
            char * dirname = strtok(NULL, " ");
            int result;
            if(dirname == NULL)
            {
                printf("Precisa de mais argumentos.\n");
            }
            else
            {
                result = ls_diretorio(dirname, inicio_dados); 

            if (!result)
                printf("\nArquivo não encontrado ou problema ao excluir o arquivo. Tente novamente.\n");
            }
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
            //printf("\n Nome do diretorio: %s", arquivo);
            registraAdmin(arquivo);
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
    Celula * bloco_aux;
    int i, inicio_dados = 56;
    char new_path[strlen(arquivo)+12]; 

    for (int i = 0; i < strlen(arquivo)+12; i++)
        new_path[i] = NULL;

    strcat(new_path, arquivo);
    strcat(new_path, "/arquivo.txt");
    fp = fopen(new_path, "w");
    if (fp != NULL)
    {
        printf("\nAbri o arquivo");
        
        /* Primeira linha: bitmap */
        for (i = 0; i < BLOCOS; i++)
            fprintf(fp, "%d", bitmap[i]);

        fputc('\n', fp);

        /* Segunda linha linha: bitmap */
        for (i = 0; i < BLOCOS; i++)
            fprintf(fp, "%5d", FAT[i]->prox);
        
        printf("\nGRAVAMOS %d BLOCOS\n", i);

        /* Próximas linhas: dados em disco */
        /* Blocos de metadados - irnformacoes de arquivos e diretorios */
        /* for(i = inicio_dados; i < (inicio_dados + 4882); i++)
        {
            bloco_aux = (Bloco *) FAT[i]->endereco;
            if (bloco_aux != NULL)
            {
                fprintf(fp, "|%s|%c|%d", bloco_aux->nome, bloco_aux->tipo, bloco_aux->tamanho);
                fprintf(fp, "|%d|%d", bloco_aux->pos_fat, bloco_aux->filhos);
                fprintf(fp, "|%s", asctime(localtime(bloco_aux->t_criado)));
                fprintf(fp, "|%s", asctime(localtime(bloco_aux->t_acesso)));
                fprintf(fp, "|%s", asctime(localtime(bloco_aux->t_alterado)));
                fprintf(fp, "|%d", bloco_aux->node_filho->pos_fat);
                fprintf(fp, "|%d", bloco_aux->node_prox->pos_fat);
            }            
            
            fputc('\n', fp);
        } */
        
        /* Blocos de dados - informações em texto */
        /* for (i = i; i < BLOCOS; i++)
        {
            if (FAT[i] != -1)
            {
                fprintf(fp, "%s", (char *) admin[i]);
            }
            fputc('\n', fp);
        } */
        fclose(fp);
    }
    
    //fseek( fp, 0, SEEK_CUR );
    printf("\nSai!");
    
    return 1;
}

int remove_filhos(Celula * pai) {

    if(pai == NULL)
        return 1;

    int indice = pai->pos_fat;
    int ok = 0;

    if(!pai->node_filho)
        return 1;

    Celula * aux = pai->node_filho;
    Celula * ant = NULL;

    while(aux) {
        if(aux->tipo == 'A') {

            // CORREÇÃO DA LISTA LIGADA
            // quer dizer que não é mais o primeiro filho, e seria o atual
            if(aux->node_prox)
                pai->node_filho = aux->node_prox;
            else
                pai->node_filho = NULL;

            indice = aux->pos_fat;
            printf("\nPosicao na fat do arquivo a ser deletado: %d\n", indice);
            int atual;
            
            while (FAT[indice]->prox != -1)
            {
                atual = indice;
                indice = FAT[indice]->prox;
                admin[atual] = NULL;

                FAT[atual]->endereco = NULL;
                FAT[atual]->prox = -1;
                bitmap[atual] = 1;
            }

            admin[indice] = NULL;
            FAT[indice]->endereco = NULL;
            bitmap[indice] = 1;

        }
        else if(aux->tipo == 'D') {
            // CORREÇÃO DA LISTA LIGADA
            // quer dizer que não é mais o primeiro filho, e seria o atual
            if(aux->node_prox)
                pai->node_filho = aux->node_prox;
            else 
                pai->node_filho = NULL;

            ok = remove_filhos(aux); // remoção de arquivos e diretórios filhos

            // ELIMINAÇÃO DA CÉLULA

            indice = aux->pos_fat;
            printf("\nPosicao na fat do diretório a ser deletado: %d\n", indice);
            int atual;
            
            while (FAT[indice]->prox != -1)
            {
                atual = indice;
                indice = FAT[indice]->prox;
                admin[atual] = NULL;

                FAT[atual]->endereco = NULL;
                FAT[atual]->prox = -1;
                bitmap[atual] = 1;
            }

            free(aux->nome);
            aux->filhos = 0;
            aux->pos_fat = -1;
            aux->node_filho = NULL;

            admin[indice] = NULL;
            FAT[indice]->endereco = NULL;
            bitmap[indice] = 1;

        }

        ant = aux;
        aux = aux->node_prox;
        ant->node_prox = NULL;
        free(ant);
    }

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

    for(aux = 0; aux < 24414 && !bitmap[aux]; aux++);

    if(aux == 24414)
        return -1;
    
    return aux;
}

Celula * find_dir(char * nome, Celula * raiz) {
    Celula * aux = raiz;
    int achou = 0;

    if(!strcmp(nome, raiz->nome)) // é o "/"
        return raiz;

    char * token = strtok(nome, "/");
    //printf("%s\n", token);

    while(token && !achou) {
        aux = aux->node_filho;

        while(aux && !achou) {
            if(aux->tipo == 'D' && !strcmp(token, aux->nome))
                achou = 1;
            else
                aux = aux->node_prox;
        }
        
        if(aux) {
            //printf("%s\n", token);
            //getchar();
            achou = 0;
            token = strtok(NULL, "/");
        }
        else token = NULL;
    }

    return aux;
}

// CUIDADO!!!! ELE ALTERA O PATH PARA ALGO SEM O / DPS!!!!!!!!!
char * nome_arquivo(char * path) {  // Devolve nome do arquivo dado por um path

    char * token = strtok(path, "/");
    printf("\n%s ", token);
    char * past;

    while(token) {
        past = token;
        printf("- %s", token);
        token = strtok(NULL, "/");
    }

    printf(": %s\n", past);

    return past;
}

Celula * devolve_ult(Celula * pai) {
    Celula * aux  = pai->node_filho;

    while(aux->node_prox)
        aux = aux->node_prox;

    return aux;
}

int ls_diretorio(char * diretorio, int inicio_dados)
{
    Celula * raiz, * aux, * aux_ant;
    int pos_fat, atual;
    raiz = admin[inicio_dados]; //recebendo o /

    char * caminho = strtok(diretorio, "/");
    char * proximo = strtok(NULL, "/");

    aux = raiz->node_filho;
    
    if (aux == NULL)
    {
        aux = raiz->node_prox;

        if (aux == NULL)
            return 0;
    }
    /* percorrendo caminho enviado pelo arquivo */
    while( proximo != NULL )
    {
        printf( " %s\n", caminho);
        /* percorrendo lista de filhos */
        while (aux->node_filho != NULL && strcmp(aux->nome, caminho) != 0 )
        {
            aux = aux->node_filho;
        }

        if (aux == NULL)
            return 0;
        
        caminho = proximo;
        proximo = strtok(NULL, "/");
    }

    /* encontrei a pasta do arquivo, percorro a lista de irmaos
       para encontrar o arquivo.*/
   
    //aux = aux->node_filho;
    
    while (aux != NULL)
    {
        printf("\n %s     \t\t %dKB \t %c", aux->nome, aux->tamanho, aux->tipo);
        aux = aux->node_prox;
    }
    printf("\n");
    return 1;
}

int rm_arquivo(char * arquivo, int inicio_dados)
{
    Celula * raiz, * aux, * aux_ant;
    int pos_fat, atual;
    //Bloco * bloco;
    raiz = admin[inicio_dados]; //recebendo o /
    printf("\nRaiz: %s", raiz->nome);

    char * caminho = strtok(arquivo, "/");
    char * proximo = strtok(NULL, "/");

    printf("\nCaminho: %s proximo: %s", caminho, proximo);
    aux_ant = raiz;
    printf("\nOlá");
    aux = raiz->node_filho;
    printf("\naux: %p", aux);
    
    /* percorrendo caminho enviado pelo arquivo */
    while( proximo != NULL )
    {
        printf( " %s\n", caminho);
        /* percorrendo lista de filhos */
        while (aux->node_filho != NULL && strcmp(aux->nome, caminho) != 0 )
        {
            aux_ant = aux;
            aux = aux->node_filho;
        }

        if (aux == NULL)
            return 0;
        
        caminho = proximo;
        proximo = strtok(NULL, "/");
        
    }
    printf("\nOpa, passei - e devia ter passado mesmo");
    /* encontrei a pasta do arquivo, percorro a lista de irmaos
       para encontrar o arquivo.*/
    if (aux->node_prox != NULL)
    {
        aux_ant = aux;
        aux = aux->node_prox;
        printf("\n Aqui não preciso entrar");
    }
    printf("\n Passei por onde não precisava entrar");
    printf("\n Comparação: %d", strcmp(aux->nome, caminho));
    printf("\n Antes da comparação, valor de aux: %p", aux);
    while (aux != NULL && strcmp(aux->nome, caminho) != 0 )
    {
        aux_ant = aux;
        aux = aux->node_filho;
    }
    printf("\n Depois da comparação");
    
    if (aux == NULL)
        return 0;
    else /* Encontrei o arquivo, vamos apagá-lo */
    {
        printf("\n Encontrei o arquivo");
        printf("\n auxant %p nome %s  -- auxprox %p", aux_ant, aux_ant->nome, aux->node_prox);
         /* retira registro do diretorio anterior */
         /* caso o anterior seja um irmão */
        if (aux_ant->node_prox == aux)
            aux_ant->node_prox = aux->node_prox;
        /* caso o anterior seja uma mãe */
        if (aux_ant->node_filho == aux)
            aux_ant->node_filho = aux->node_prox;

        /* retira registro na FAT e bitmap */
        pos_fat = aux->pos_fat;
        printf("\nPosicao na fat do arquivo a ser deletado: %d", pos_fat);
        
        while (FAT[pos_fat]->prox != -1)
        {
            atual = pos_fat;
            pos_fat = FAT[pos_fat]->prox;
            /* essa seria a parte de apagar a entrada em admin[x] */
            /* free(FAT[atual]->endereco)*/
            FAT[atual]->prox = -1;
            bitmap[atual] = 1;
        }

        bitmap[pos_fat] = 1;


    }
    

    return 0;
}

int rm_diretorio(char * diretorio, int inicio_dados)
{
    /* por enquanto está como o rm_arquivo */
    Celula * raiz, * aux, * aux_ant;
    int pos_fat, atual;
    //Bloco * bloco;
    raiz = admin[inicio_dados]; //recebendo o /
    printf("\nRaiz: %s", raiz->nome);

    char * caminho = strtok(diretorio, "/");
    char * proximo = strtok(NULL, "/");

    printf("\nCaminho: %s proximo: %s", caminho, proximo);
    aux_ant = raiz;
    printf("\nOlá");
    aux = raiz->node_filho;
    printf("\naux: %p", aux);
    
    /* percorrendo caminho enviado pelo arquivo */
    while( proximo != NULL )
    {
        printf( " %s\n", caminho);
        /* percorrendo lista de filhos */
        while (aux->node_filho != NULL && strcmp(aux->nome, caminho) != 0 )
        {
            aux_ant = aux;
            aux = aux->node_filho;
        }

        if (aux == NULL)
            return 0;
        
        caminho = proximo;
        proximo = strtok(NULL, "/");
        
    }
    printf("\nOpa, passei - e devia ter passado mesmo");
    /* encontrei a pasta do arquivo, percorro a lista de irmaos
       para encontrar o arquivo.*/
    if (aux->node_prox != NULL)
    {
        aux_ant = aux;
        aux = aux->node_prox;
        printf("\n Aqui não preciso entrar");
    }
    printf("\n Passei por onde não precisava entrar");
    printf("\n Comparação: %d", strcmp(aux->nome, caminho));
    printf("\n Antes da comparação, valor de aux: %p", aux);
    while (aux != NULL && strcmp(aux->nome, caminho) != 0 )
    {
        aux_ant = aux;
        aux = aux->node_filho;
    }
    printf("\n Depois da comparação");
    
    if (aux == NULL)
        return 0;
    else /* Encontrei o arquivo, vamos apagá-lo */
    {
        printf("\n Encontrei o arquivo");
        printf("\n auxant %p nome %s  -- auxprox %p", aux_ant, aux_ant->nome, aux->node_prox);
         /* retira registro do diretorio anterior */
         /* caso o anterior seja um irmão */
        if (aux_ant->node_prox == aux)
            aux_ant->node_prox = aux->node_prox;
        /* caso o anterior seja uma mãe */
        if (aux_ant->node_filho == aux)
            aux_ant->node_filho = aux->node_prox;

        /* retira registro na FAT e bitmap */
        pos_fat = aux->pos_fat;
        printf("\nPosicao na fat do arquivo a ser deletado: %d", pos_fat);
        
        while (FAT[pos_fat]->prox != -1)
        {
            atual = pos_fat;
            pos_fat = FAT[pos_fat]->prox;
            /* essa seria a parte de apagar a entrada em admin[x] */
            /* free(FAT[atual]->endereco)*/
            FAT[atual]->prox = -1;
            bitmap[atual] = 1;
        }

        bitmap[pos_fat] = 1;


    }

    return 0;
}