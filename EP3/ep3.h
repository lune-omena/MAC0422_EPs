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

    unsigned int arq_criado; // instante de tempo em que o arquivo foi criado
    unsigned int arq_alterado; // instante de tempo em que o arquivo foi alterado
    unsigned int arq_acesso; // instante de tempo em que o arquivo foi acessado

} Arquivo;


typedef struct list {
    
    unsigned int t_criado; // instante de tempo em que o arquivo foi criado
    unsigned int t_alterado; // instante de tempo em que o arquivo foi alterado
    unsigned int t_acesso; // instante de tempo em que o arquivo foi acessado

    Arquivo * arqv; // primeiro arquivo do diretório
    Arquivo * prox; // segundo arquivo do diretório 

} Diretorio;


/* PROTÓTIPOS */
char * definePrompt(); // Criaçao de prompt personalizado