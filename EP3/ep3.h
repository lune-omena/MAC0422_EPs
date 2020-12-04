/* EP1 - Sistemas Operacionais
 * Parte 1 - SHELL
 * 
 * Nome: Lara Ayumi Nagamatsu               NUSP: 9910568
 * Nome: Lucy Anne de Omena Evangelista     NUSP: 11221776
 * 
 */

// Este bloco é uma célula da FAT
typedef struct node {
    // cada char é um byte, um bloco tem 4KB
    char data[4000]; 
    struct node * prox;
} Bloco;

typedef struct cell {

    int tamanho; // tamanho em bytes, só para não diretórios
    unsigned int arq_criado; // instante de tempo em que o arquivo foi criado
    unsigned int arq_alterado; // instante de tempo em que o arquivo foi alterado
    unsigned int arq_acesso; // instante de tempo em que o arquivo foi acessado

} Arquivo;


typedef struct list {
    
    unsigned int t_criado; // instante de tempo em que o arquivo foi criado
    unsigned int t_alterado; // instante de tempo em que o arquivo foi alterado
    unsigned int t_acesso; // instante de tempo em que o arquivo foi acessado

    Arquivo * arqv;
    Arquivo * prox;
} Diretorio;


/* PROTÓTIPOS */
char * definePrompt(); // Criaçao de prompt personalizado