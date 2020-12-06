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
    Arquivo * arqv_prox; // segundo arquivo do diretório 

    struct list * dir_filho; // Diretório abaixo na hierarquia
    struct list * dir_prox; // Diretório na mesma hierarquia

} Diretorio;


/* PROTÓTIPOS */
char * definePrompt(); // Criaçao de prompt personalizado
int find_bitmap(); // Encontra espaço vago na bitmap (retorna -1 se não existe espaço)
int find_fat(); // Encontra espaço vago na FAT
Diretorio * find_dir(char * nome, Diretorio * raiz); // Encontra o diretório desejado (NULL se não existe)
int ult_arq(Diretorio * dir); // devolve o último arquivo do 
char * nome_arquivo(char * path); // Devolve nome do arquivo dado por um path