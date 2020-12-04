/* EP1 - Sistemas Operacionais
 * Parte 1 - SHELL
 * 
 * Nome: Lara Ayumi Nagamatsu               NUSP: 9910568
 * Nome: Lucy Anne de Omena Evangelista     NUSP: 11221776
 * 
 */

// eu n sei se isso tá certo
typedef struct node {
    char data[4000]; // cada char é um byte, um bloco tem 4KB
    struct node * prox;
} Bloco;


/* PROTÓTIPOS */
char * definePrompt(); // Criaçao de prompt personalizado