/* EP1 - Sistemas Operacionais
 * Parte 1 - SHELL
 * 
 * Nome: Lara Ayumi Nagamatsu               NUSP: 9910568
 * Nome: Lucy Anne de Omena Evangelista     NUSP: 11221776
 * 
 */

// Este bloco é uma célula da FAT
typedef struct node {

    // endereço do bloco
    void * endereco;
    // indica o índice do próximo bloco (-1 se não tiver próximo) 
    int prox;
} Bloco;

/*
typedef struct cell {
    int tamanho; // tamanho em bytes, só para não diretórios
    int pos_fat; // entrada na tabela FAT

    char * nome;
    struct cell * arqv_prox; // segundo arquivo do diretório 

    unsigned int arq_criado; // instante de tempo em que o arquivo foi criado
    unsigned int arq_alterado; // instante de tempo em que o arquivo foi alterado
    unsigned int arq_acesso; // instante de tempo em que o arquivo foi acessado

} Arquivo;


typedef struct list {

    char * nome;
    
    unsigned int t_criado; // instante de tempo em que o arquivo foi criado
    unsigned int t_alterado; // instante de tempo em que o arquivo foi alterado
    unsigned int t_acesso; // instante de tempo em que o arquivo foi acessado

    int pos_fat; // posição inicial do fat do diretório

    Arquivo * arqv; // primeiro arquivo do diretório

    struct list * dir_filho; // Diretório abaixo na hierarquia
    struct list * dir_prox; // Diretório na mesma hierarquia

} Diretorio;*/

typedef struct celula {
    
    char * nome; // Nome do arquivo txt/diretório 
    char tipo; // D: diretório, A: arquivo texto -> 1B

    unsigned int t_criado; // instante de tempo em que o arquivo foi criado  ->> 4B
    unsigned int t_alterado; // instante de tempo em que o arquivo foi alterado  -> 4B
    unsigned int t_acesso; // instante de tempo em que o arquivo foi acessado -> 4B

    int pos_fat; // posição inicial no FAT -> 4B
    int filhos; // conta o número de filhos do diretório
    int tamanho; // tamanho em bytes, só para não diretórios -> 4B

    struct celula * node_filho; // Diretório abaixo na hierarquia -> 4B
    struct celula * node_prox; // Diretório na mesma hierarquia -> 4B

} Celula;

/* PROTÓTIPOS */
char * definePrompt(); // Criaçao de prompt personalizado
int find_bitmap(); // Encontra espaço vago na bitmap (retorna -1 se não existe espaço)

int find_bitmap_dir(); // Encontra espaço vago na bitmap para METADADOS
int find_butmap_arq(); // Encontra espaço vago na bitmap para TEXTO
int find_fat(); // Encontra espaço vago na FAT
Celula * find_dir(char * nome, Celula * raiz); // Encontra o diretório desejado (NULL se não existe)
char * nome_arquivo(char * path); // Devolve nome do arquivo dado por um path
Celula * devolve_ult(Celula * pai); // Devolve a última célula filha do pai
int remove_filhos(Celula * pai);

int registraAdmin(char * arquivo, Celula * raiz);
void rec();
int recebeAdmin(char * arquivo);
int ls_diretorio(char * diretorio, int inicio_dados);
int rm_arquivo(char * arquivo, int inicio_dados);
int rm_diretorio(char * diretorio, int inicio_dados);