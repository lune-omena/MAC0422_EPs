/* EP1 - Sistemas Operacionais
 * Parte 2 - Threads
 * 
 * Nome: Lara Ayumi Nagamatsu               NUSP: 9910568
 * Nome: Lucy Anne de Omena Evangelista     NUSP: 11221776
 * 
 */

/* ./ep1 <escalonador> <arquivo_trace> <novo_arquivo> <d>
 * sendo <d> opcional! */

/* OBS: TEREMOS QUE CRIAR UM ARQUIVO TXT DE NOME <arg[3]> CONTENDO:
 * NOME TF TR
 * NOME = NOME DO PROCESSO
 * TF = INSTANTE DE TEMPO QUE O PROCESSO TERMINOU
 * TR = TEMPO DE RELÓGIO (TF-T0)
 * E NO FIM UMA LINHA CONTENDO UM ÚNICO NÚMERO QUE INFORMA A
 * QUANTIDADE DE MUDANÇAS DE CONTEXTO EFETUADAS NA SIMULAÇÃO
 * 
 * E SE O USUÁRIO OPTAR POR USAR O COMANDO "d", TEREMOS QUE INDICAR
 * 
 */

/* Bibliotecas */
#include "ep1.h"     /* header */
#include <stdio.h>   /* printf(), fopen()... */
#include <string.h>  /* strlen(), strtok() */
#include <stdlib.h>  /* atoi() */
#include <time.h>    /* usado para ver quanto tempo decorreu no prorgrama */
#include <pthread.h> /* pthread_init(), pthread_mutex_lock()...*/
#include <stdlib.h>
#include <unistd.h>

/* VARIÁVEIS GLOBAIS */
pthread_mutex_t mutex;
pthread_mutex_t mutex_proc; // para aguardar o processo atual terminar

int main(int argc, char ** argv)
{
    /* PROCESSOS */
    int num_p;                      // número de processos do arquivo
    Data * processos;               // vetor contendo processos

    /* TIPOS DE ESCALONADOR */
    int escalonador = -1;

    if(argv[2] == NULL) {
        printf("Você precisa inserir um arquivo txt como segundo parâmetro!\n");
        exit(EXIT_FAILURE);
    }

    /* LEITURA DE ARQUIVO + PROCESSOS */
    num_p = contaLinhas(argv[2]);
    processos = (Data *) malloc(num_p*sizeof(Data));
    armazenaProcessos(argv[2], processos);

    for(int i = 0; i < num_p; i++) 
        printf("%s %d %d %d\n", processos[i].processo, processos[i].d0, processos[i].dt, processos[i].deadline);

    printf("\n");

    if(argv[1] != NULL)
        escalonador = atoi(argv[1]);

    /* ESCALANDO PROCESSOS */
    switch(escalonador) {
        case(1):
            printf("ESCALONADOR: First Come, First Served\n");
            FCFS(processos, num_p);
            break;
        case(2):
            printf("ESCALONADOR: Shortest Remaining Time Next\n");
            break;
        case(3):
            printf("ESCALONADOR: Round-Robin\n");
            break;
        default:
            printf("Escalonador não reconhecido.\n");
    }

    /* Liberando memória */
    for(int i = 0; i < num_p; i++)
        free(processos[i].processo);    
    free(processos);

    printf("\nFIM DO PROGRAMA\n");

    return 0;
}

int contaLinhas(char * arquivo)
{
    int count = 0;
    char ch;
    FILE * f;

    if ((f = fopen(arquivo, "r")) == NULL) {
      perror("\nHouve um erro ao abrir o arquivo!\n");
      exit(EXIT_FAILURE);
    }

    while(!feof(f)) {
        ch = fgetc(f);
        if(ch == '\n')
            count++;
    }

    fclose(f);
    printf("%d PROCESSOS COMPUTADOS\n", count);

    return count;
}

void armazenaProcessos(char * arquivo, Data * processos)
{
    FILE *f;
    char buf[MAX];      // guarda a string da linha
    char * buf_break;   // guarda a string entre " "
    int i;              // contador de linhas para a posição no vetor
    int j;              // contador dentro da linha a partir da separação " "
    int size;           // tamanho da string

    f = fopen(arquivo, "r");
    i = 0;

    /* abaixo lê as linhas do arquivo */
    while( fgets (buf, MAX, f)!= NULL ) {
        //printf(".%s. tem tamanho %ld, é o processo %d\n", buf, strlen(buf), i);
        
        buf_break = strtok(buf, " ");
        
        j = 0;
        while(buf_break != NULL) {
            switch(j) {
                case 0: // nome do processo
                    size = strlen(buf_break)+1;
                    //printf("%s, tamanho: %d\n", buf_break, size);
                    strcat(buf_break, "\0");
                    processos[i].processo = (char *) malloc(size*sizeof(char));
                    strcpy(processos[i].processo, buf_break);
                    break;
                case 1: // t0
                    processos[i].d0 = atoi(buf_break);
                    break;
                case 2: // dt
                    processos[i].dt = atoi(buf_break);
                    break;
                case 3: // deadline
                    processos[i].deadline = atoi(buf_break);
                    break;
                default:
                    printf("Algo deu errado. :(\n");
                    exit(EXIT_FAILURE);
            };

            buf_break = strtok(NULL, " ");
            j++;
        }

        i++;
    }

    fclose(f);
}

/* VARIÁVEIS GLOBAIS */
long int tempo_decorrido = 0; //tempo decorrido do processo
long int tempo_dt = 0;  //tempo a decorrer do processo i.e. tf -t0
long int tempo_prog;
long int x = 0;

// antiga void * existe
void * thread(void *a)
{
    /* quero receber a duração da thread*/
    /* preciso receber o inicio dela e retornar o final? talvez sim, talvez não o.o*/
    long int duracao = tempo_dt; /* convertendo parametro de entrada */
    printf("A duração é de %ld segundos\n", duracao);
    long int i;

    for(i = 0; i < duracao; i++) {
        /* PROTOCOLO DE ENTRADA*/
        pthread_mutex_lock(&mutex); // P() -> espera valor de mutex 1 e decrementa
        /* REGIÃO CRITICA */
        x++;
        sleep(1);
        printf("Rodando por %ld de tempo...\n", i);
        tempo_decorrido++;
        tempo_prog++;
        /* PROTOCOLO DE SAIDA */
        pthread_mutex_unlock(&mutex); // V() -> incrementa após P()
    }

    return NULL;
}

void FCFS(Data * processos, int num_p) {
    /* ordena processos prontos em fila por ordem de chegada e executa nessa ordem */

    int ind = 0;                     // índice do processo no vetor processos
    pthread_t tid[num_p];            // vetor de threads
    int i;
    int terminou = 0;
    int iniciou = 0;
    tempo_prog = 0;                  //tempo decorrido do programa

    /* enquanto não acabou de rodar todos os processos
     * eu vou esperando numa fila, vou atualizar a variável ind como representante
     * do índice no vetor de processos. Como é o FCFS vai funcionar como uma fila,
     * e vou considerar que só tem 1 CPU, portanto só um processo pode ocorrer por
     * vez. (p.ex. 2 processos, um de 2 seg e outro de 5 seg, vai rodar no total
     * 7 segundos!)
    */

    pthread_mutex_init(&mutex, NULL);
    pthread_mutex_init(&mutex_proc, NULL);

    // ABAIXO: recebo na variável global tempo_dt o tempo a se decorrer do processo e crio thread
    while(ind < num_p) {
        pthread_mutex_lock(&mutex_proc);
        terminou = 0;
        tempo_dt = processos[ind].dt;
        
        // preciso esperar quanto tempo der até acionar a thread no t0 dela
        while(!iniciou) {
            if(tempo_prog >= processos[ind].d0)
                iniciou = 1;
            else {
                sleep(1);
                tempo_prog++;
            }
        }

        if (pthread_create(&tid[ind], NULL, thread, NULL)) {
            printf("\n ERROR creating thread\n");
            exit(1);
        }
        while(!terminou) {
            //printf("VAI DESGRAÇA\n");
            /*
            printf("tempo decorrido:%ld e tempo_dt: %ld\n", tempo_decorrido, tempo_dt);
            sleep(1);*/
            if(tempo_decorrido >= tempo_dt) {
                printf("Esperou por %ld\n", tempo_decorrido);
                ind++;
                tempo_decorrido = 0;
                pthread_mutex_unlock(&mutex_proc);
                terminou = 1;
            }
        }
    }

    /* Esperando todas as threads executarem */
    for (i = 0; i < num_p; i++)
        if (pthread_join(tid[i], NULL)) {
            printf("\n Erro ao juntar a thread!");
            exit(1);
        }

    pthread_mutex_destroy(&mutex);
    pthread_mutex_destroy(&mutex_proc);

}