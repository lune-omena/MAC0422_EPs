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
#include <sched.h>  /* sched_getcpu() para pegar CPU */

/* VARIÁVEIS GLOBAIS */
pthread_mutex_t mutex;          // aguardar proceso rodar sua seção crítica
pthread_mutex_t mutex_proc;     // para aguardar o processo atual (da lista de processos) terminar
long int tempo_decorrido = 0;   // tempo decorrido do processo
long int tempo_dt = 0;          // tempo a decorrer do processo i.e. tf -t0
long int tempo_prog;            // tempo decorrido do programa
long int x = 0;                 // variável usada para consumir CPU

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
            SRTN(processos, num_p);
            break;
        case(3):
            printf("ESCALONADOR: Round-Robin\n");
            RR(processos, num_p);
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

// antiga void * existe
void * thread(void *a)
{
    long int duracao = tempo_dt;
    int CPU;
    printf("A duração é de %ld segundos\n", duracao);
    long int i;

    /* CPU USADA NESTA THREAD */
    CPU = sched_getcpu();

    for(i = 0; i < duracao; i++) {
        /* PROTOCOLO DE ENTRADA*/
        pthread_mutex_lock(&mutex); // P() -> espera valor de mutex 1 e decrementa
        /* REGIÃO CRITICA */
        x++;
        sleep(1);
        printf("Rodando por %ld de tempo...\n", i+1);
        printf("Usando CPU %d\n", CPU);
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
    int i;                           // iterando do join
    int terminou = 0;
    int iniciou = 0;
    tempo_prog = 0;                  // tempo decorrido do programa

    /* enquanto não acabou de rodar todos os processos
     * eu vou esperando numa fila, vou atualizar a variável ind como representante
     * do índice no vetor de processos. Como é o FCFS vai funcionar como uma fila,
     * e vou considerar que só tem 1 CPU, portanto só um processo pode ocorrer por
     * vez. (p.ex. 2 processos, um de 2 seg e outro de 5 seg, vai rodar no total
     * 7 segundos!)
    */

   /* caso a gente for respeitar deadlines pra processos de mesmo t0, sugiro
    * que façamos um novo vetor de processos para rearranjar a partir de deadline 
    * aí, seria só manter a mesma estrutura:
    * 1. malloca novo_processos(tamanho do processos)
    * 2. itera sobre processos:
    * 3. checa os processos com mesmo t0 (while t0 de processos[ind] == t0 de ind +1)
    * 4. ordena por deadline
    */

    pthread_mutex_init(&mutex, NULL);
    pthread_mutex_init(&mutex_proc, NULL);

    // ABAIXO: recebo na variável global tempo_dt o tempo a se decorrer do processo e crio thread
    while(ind < num_p) {
        pthread_mutex_lock(&mutex_proc);
        terminou = 0;
        tempo_dt = processos[ind].dt;
        
        // esperar até acionar a thread no t0 dela
        while(!iniciou) {
            if(tempo_prog >= processos[ind].d0)
                iniciou = 1;
            else {
                sleep(1);
                tempo_prog++;
            }
        }

        // cria thread
        if (pthread_create(&tid[ind], NULL, thread, NULL)) {
            printf("\n ERROR creating thread\n");
            exit(1);
        }

        // trava a thread
        while(!terminou) {
            //printf("VAI DESGRAÇA\n");
            /*
            printf("tempo decorrido:%ld e tempo_dt: %ld\n", tempo_decorrido, tempo_dt);
            sleep(1);*/
            if(tempo_decorrido >= tempo_dt) {
                printf("Esperou por %ld\n", tempo_decorrido);
                if(!pthread_cancel(tid[ind]))
                    printf("a thread foi destruída! c:\n"); // mata a thread
                ind++;
                tempo_decorrido = 0;
                pthread_mutex_unlock(&mutex_proc);
                terminou = 1;
                //pthread_cleanup_push()??
            }
        }
    }

    /* Esperando todas as threads executarem */
    // não sei se é necessário fazer o join...?aaaas
    for (i = 0; i < num_p; i++)
        if (pthread_join(tid[i], NULL)) {
            printf("\n Erro ao juntar a thread!");
            exit(1);
        }

    pthread_mutex_destroy(&mutex);
    pthread_mutex_destroy(&mutex_proc);

}

void SRTN(Data * processos, int num_p) {
    /* Ordena os processos prontos em uma fila por ordem do tempo de execução deles. 
     * Do mais curto para o mais longo e executa nessa ordem */
    /* O tempo de execução dele é comparado com o tempo que falta do processo que
     * está sendo executado. Se o novo processo é mais curto, ele passa a executar 
     * e o atual vai pra fila de prontos para continuar sua execução depois */

    int ind = 0;                     // índice do processo no vetor processos
    pthread_t tid[num_p];            // vetor de threads
    tempo_prog = 0;                  // tempo decorrido do programa
    int iniciou = 0;
    int terminou = 0;

    /* FILA */
    int i;
    Data * fila = (Data *) malloc(num_p*sizeof(Data));
    int front = 0;
    int rear = -1;
    int num_prontos = 0;

    /* DECISÃO DE PROJETO: COLOCA NA FRENTE OU ATRÁS DA FILA? acho melhor no começo da fila */
    /* 
     * 
     * em tempo de execução, leio os processos que têm tempo t0 == tempo_prog
     * coloco-os em ordem na fila;
     * checo se o primeiro tem tempo de duração menor do que o tempo_decorrido do que roda;
     * se sim, pauso o atual + troco dt dele para oq resta e troco a posição dos dois na fila;
     * itera.
     *   
     */

    pthread_mutex_init(&mutex, NULL);
    pthread_mutex_init(&mutex_proc, NULL);

    while(!terminou) {

        // ESPERA OS PROCESSOS DE T0 == TEMPO_PROG
        while(!iniciou) {
            if(tempo_prog >= processos[ind].d0)
                iniciou = 1;
            else {
                sleep(1);
                tempo_prog++;
            }
        }

        // PEGA TODOS OS PROCESSOS DE T0 == TEMPO_PROG
        while(tempo_prog >= processos[ind].d0) {
            // adiciona na fila 
            // atualiza o índice do último item da fila;
            if( rear == -1 || rear == front ) {// fila vazia
                // checa se o dt é menor que o do atual
                // código aqui
                // SE NÃO FOR, BOTA NA FILA 
                fila[++rear] = processos[ind];
            }
            else {
                // PRIMEIRO CHECA SE DT É MENOR QUE DO ATUAL 
                // código aqui
                // SE NÃO FOR, COMPARA COM O RESTO DA FILA
                for(i = front; i < rear && processos[ind].dt < fila[i].dt; i++);
                if(processos[ind].dt < fila[i].dt) {
                    fila[++rear] = fila[i];
                    fila[i] = processos[ind];
                }
            }

            ind++;
        }

        // quando for botar de volta na fila, vou colocar na posição [front-1]
        // pq n tem problema já que necessáriamente vai ser um número > 0

        // cria thread se o atual morreu já
        if (pthread_create(&tid[ind], NULL, thread, NULL)) {
            printf("\n ERROR creating thread\n");
            exit(1);
        }

        ind++;
    }

    /* Esperando todas as threads executarem */
    // não sei se é necessário fazer o join...?aaaas
    for (int i = 0; i < num_p; i++)
        if (pthread_join(tid[i], NULL)) {
            printf("\n Erro ao juntar a thread!");
            exit(1);
        }

    pthread_mutex_destroy(&mutex);
    pthread_mutex_destroy(&mutex_proc);

}

void RR(Data * processos, int num_p) {

}