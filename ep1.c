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
 * 
 */

/* Bibliotecas */
#include "ep1.h"     /* header */
#include <stdio.h>   /* printf(), fopen()... */
#include <string.h>  /* strlen(), strtok() */
#include <stdlib.h>  /* atoi() */
#include <time.h>    /* usado para ver quanto tempo decorreu no prorgrama */
#include <stdlib.h>
#include <unistd.h>
#include <sched.h>  /* sched_getcpu() para pegar CPU */
#include <errno.h> /* número do erro*/

/* VARIÁVEIS GLOBAIS */
pthread_mutex_t mutex;          // aguardar proceso rodar sua seção crítica
pthread_mutex_t mutex_proc;     // para aguardar o processo atual (da lista de processos) terminar
long int tempo_decorrido = 0;   // tempo decorrido do processo
long int tempo_dt = 0;          // tempo a decorrer do processo i.e. tf -t0
long int tempo_prog;            // tempo decorrido do programa
long int x = 0;                 // variável usada para consumir CPU
int d_option = 0;
long int thread_id_num = -1;    // id do processo
Data *thread_c;                 // conteúdo dos processos
int ind_espera = -1;
int ind_proc_atual = -1;
int total;                      //número total de processos
char *arq_trace;

int main(int argc, char ** argv)
{
    /* PROCESSOS */
    int num_p;                      // número de processos do arquivo
    Data * processos;               // vetor contendo processos

    /* TIPOS DE ESCALONADOR */
    int escalonador = -1;

    if(argc < 3 || argc > 5)
    {
        printf("Entrada inválida!");
        exit(EXIT_FAILURE);
    }

    if(argv[2] == NULL) {
        printf("Você precisa inserir um arquivo txt como segundo parâmetro!\n");
        exit(EXIT_FAILURE);
    }
    else
    {
        arq_trace = argv[3];
    }

    if(argc == 5 && *argv[4] == 'd')
    {
        printf("Opção de exibição de eventos adicionais acionada!\n");
        d_option = 1;
    }

    /* LEITURA DE ARQUIVO + PROCESSOS */
    num_p = contaLinhas(argv[2]);
    processos = (Data *) malloc(num_p*sizeof(Data));
    armazenaProcessos(argv[2], processos);
    inicializaRegistros(arq_trace);

    printf("\nProcessos lidos:\n");
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
            printf("Não implementado.\n");
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
        
        buf_break = strtok(buf, " ");
        
        j = 0;
        while(buf_break != NULL) {
            switch(j) {
                case 0: // nome do processo
                    size = strlen(buf_break)+1;
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

void inicializaRegistros(char * arquivo)
{
    FILE *arq;

    arq = fopen(arquivo, "w");

    fclose(arq);

    return;  
}

void registraFim(char * arquivo, int mudancas)
{
    FILE *arq;

    arq = fopen(arquivo, "a");
    
    if (!arq)
    {
        printf("\n\tErro ao abrir ou criar arquivo!");
		return;
    }

    fprintf(arq, "%d\n", mudancas);

    fclose(arq);
    return;   
}

void registraProcessos(char * arquivo, char * nome, int tf, int tr)
{
    FILE *arq;

    arq = fopen(arquivo, "a");
    
    if (!arq)
    {
        printf("\n\tErro ao abrir ou criar arquivo!");
		return;
    }

    fprintf(arq, "%s %d %d\n", nome, tf, tr);

    fclose(arq);
    return;   
}

// função de thread para FCFS
void * thread(void *a)
{
    long int duracao = tempo_dt;
    int CPU;
    printf("A duração é de %ld segundos\n", duracao);
    long int i;

    /* CPU USADA NESTA THREAD */

    for(i = 0; i < duracao; i++) {
        /* PROTOCOLO DE ENTRADA*/
        pthread_mutex_lock(&mutex); // P() -> espera valor de mutex 1 e decrementa
        /* REGIÃO CRITICA */
        x++;
        sleep(1);
        CPU = sched_getcpu();

        if(d_option) {
            if(ind_espera < total && thread_c[ind_espera].d0 <= tempo_prog) {
                fprintf(stderr, "O processo %s de t0=%d dt=%d deadline=%d chegou.\n", 
                thread_c[ind_espera].processo, thread_c[ind_espera].d0,
                thread_c[ind_espera].dt, thread_c[ind_espera].deadline);
                ind_espera++;
            }
            fprintf(stderr,"Processo %ld usando CPU %d\n", thread_id_num, CPU);
        }
        printf("Rodando por %ld de tempo...\n", i+1);

        tempo_decorrido++;
        tempo_prog++;
        /* PROTOCOLO DE SAIDA */
        pthread_mutex_unlock(&mutex); // V() -> incrementa após P()
    }
    if(d_option) {
        fprintf(stderr, "Processo %ld deixou de usar a CPU %d.\n", thread_id_num, CPU);
        fprintf(stderr, "Processo %ld acabou e se encontra na linha %d do arquivo trace.\n", thread_id_num, ind_proc_atual);
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
    ind_proc_atual = 0;
    ind_espera = 0;
    total = num_p;

    if(d_option) {
        thread_c = (Data *) malloc(num_p*sizeof(Data));
        for(int j = 0; j < num_p; j++)
            thread_c[j] = processos[j];
    }

    /* enquanto não acabou de rodar todos os processos
     * espera numa fila, vou atualizar a variável ind como representante
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
        } else {
            thread_id_num = tid[ind];
            cpu_set_t cpuset;
            CPU_ZERO(&cpuset);
            CPU_SET(0, &cpuset);
  
            pthread_setaffinity_np(tid[ind], sizeof(cpu_set_t), &cpuset);
        }

        // trava a thread
        while(!terminou) {
            if(tempo_decorrido >= tempo_dt) {
                printf("Esperou por %ld\n", tempo_decorrido);
                /* Registrando valores */
                registraProcessos(arq_trace, processos[ind].processo, tempo_prog, tempo_decorrido);
                if(!pthread_cancel(tid[ind]))
                    printf("a thread foi destruída! c:\n"); // mata a thread
                ind++;
                ind_proc_atual = ind;
                tempo_decorrido = 0;
                pthread_mutex_unlock(&mutex_proc);
                terminou = 1;
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

    if(d_option)
        free(thread_c);

    pthread_mutex_destroy(&mutex);
    pthread_mutex_destroy(&mutex_proc);

}

/* VARIÁVEIS GLOBAIS PRO SRTN */
pthread_mutex_t * m_procs;
pthread_mutex_t m_escalonador;
pthread_cond_t c_escalonador = PTHREAD_COND_INITIALIZER;
pthread_cond_t * c_procs;
pthread_mutex_t pare;
int mud_cont = 0;
int * dt_exec;  // tempo de execução de cada uma das threads
int ind_proc;

void * thread_srtn(void *a)
{
    long int duracao = tempo_dt;
    int CPU;
    int indice = ind_proc;
    //printf("A duração é de %ld segundos\n", duracao);
    long int i;

    pthread_mutex_lock(&pare);
    pthread_cond_signal(&c_escalonador);
    pthread_cond_wait(&c_procs[indice], &pare);
    printf("DENTRO DA THREAD: %d\n", indice);
    pthread_mutex_unlock(&pare);

    for(i = 0; i < duracao; i++) {
        /* PROTOCOLO DE ENTRADA*/
        pthread_mutex_lock(&pare); // P() -> espera valor de mutex 1 e decrementa
        /* CPU USADA NESTA THREAD */
        CPU = sched_getcpu();
        /* REGIÃO CRITICA */
        x++;
        if(d_option)
            fprintf(stderr, "Processo %ld usando CPU %d\n", thread_id_num, CPU);

        pthread_cond_wait(&c_procs[indice], &pare);

        /* PROTOCOLO DE SAIDA */
        pthread_mutex_unlock(&pare); // V() -> incrementa após P()
    }

    if(d_option) {
        fprintf(stderr, "Processo %ld deixou de usar a CPU %d.\n", thread_id_num, CPU);
    }

    return NULL;
}

void SRTN(Data * processos, int num_p) {
    /* Ordena os processos prontos em uma fila por ordem do tempo de execução deles. 
     * Do mais curto para o mais longo e executa nessa ordem;
     * O tempo de execução dele é comparado com o tempo que falta do processo que
     * está sendo executado. Se o novo processo é mais curto, ele passa a executar 
     * e o atual vai pra fila de prontos para continuar sua execução depois */

    int ind = 0;                     // índice do vetor de threads criadas
    int ind_prontos = 0;             // número no vetor processos de processos prontos
    pthread_t tid[num_p];            // vetor de threads
    tempo_prog = 0;                  // tempo decorrido do programa
    int tam_fila = 0;
    int * t0_processos;
    int linhas = 1;
    int d_ind_prontos = -1;

    int ind_atual = -1;

    // aloca dinâmicamente os semáforos para controle 
    m_procs = (pthread_mutex_t *) malloc(num_p*sizeof(pthread_mutex_t));
    c_procs = (pthread_cond_t *) malloc(num_p*sizeof(pthread_cond_t));
    dt_exec = (int *) malloc(num_p*sizeof(int));
    t0_processos = (int *) malloc(num_p*sizeof(int));

    /* FILA */
    Node * fila = NULL; // fila de processos prontos
    Node * aux; // iteração
    Node * ant; // iteração

    if(d_option) {
        thread_c = (Data *) malloc(num_p*sizeof(Data));
        for(int j = 0; j < num_p; j++)
            thread_c[j] = processos[j];
    }

    pthread_mutex_init(&mutex, NULL);
    pthread_mutex_init(&m_escalonador, NULL);

    for(int i = 0; i < num_p; i++) {
        pthread_mutex_init(&m_procs[i], NULL);
        dt_exec[i] = 0;
    }

    // cria threads dormindo, serão acionadas com a condição de índice i
    for(int i = 0; i < num_p; i++) {
        pthread_mutex_lock(&m_escalonador);
        tempo_dt = processos[i].dt;
        ind_proc = i;
        pare = m_procs[i];
        if (pthread_create(&tid[i], NULL, thread_srtn, NULL)) {
            printf("\n ERROR creating thread\n");
            exit(1);
        } else {
            cpu_set_t cpuset;
            CPU_ZERO(&cpuset);
            CPU_SET(0, &cpuset);
  
            pthread_setaffinity_np(tid[ind], sizeof(cpu_set_t), &cpuset);
        }
        pthread_cond_wait(&c_escalonador, &m_escalonador);
        pthread_mutex_unlock(&m_escalonador);
    }
    
    int terminou = 0;
    tempo_prog++;
    while(!terminou/* ainda não foram todos os processos*/) {

        pthread_mutex_lock(&m_escalonador);
        /* checa os processos prontos a serem adicionados à fila */
        while(tempo_prog >= processos[ind_prontos].d0 && ind_prontos < num_p) {

            if(!fila) { // fila vazia
                fila = (Node *) malloc(sizeof(Node));
                fila->proc = processos[ind_prontos];
                fila->indice = ind_prontos;
                fila->estado = Espera;
                fila->prox = NULL;
                tam_fila++;
            }
            else {
                aux = fila;
                ant = NULL;

                while(aux) {
                    if(processos[ind_prontos].dt < aux->proc.dt) {
                        if(ant) {
                            insere(processos[ind_prontos], ant);
                            ant->prox->indice = ind_prontos;
                            ant->prox->estado = Espera;
                        }
                        else { // primeiro termo
                            Node * novo = (Node *) malloc(sizeof(Node));
                            novo->proc = processos[ind_prontos];
                            novo->prox = fila;
                            novo->estado = Espera;
                            novo->indice = ind_prontos;
                            fila = novo;
                        }
                        tam_fila++;
                        break;
                    }

                    ant = aux;
                    aux = aux->prox;
                }
                
                if(!aux && ant != NULL) { // último termo
                    Node * novo = (Node *) malloc(sizeof(Node));
                    novo->proc = processos[ind_prontos];
                    novo->prox = NULL;
                    novo->estado = Espera;
                    novo->indice = ind_prontos;
                    ant->prox = novo;
                    tam_fila++;
                }
            }

            ind_prontos++;
        }

        if(d_option) {
            while(d_ind_prontos != ind_prontos) {
                if(d_ind_prontos >= 0)
                    fprintf(stderr, "O processo %s de t0=%d dt=%d deadline=%d chegou.\n", 
                    processos[d_ind_prontos].processo, processos[d_ind_prontos].d0,
                    processos[d_ind_prontos].dt, processos[d_ind_prontos].deadline);
                d_ind_prontos++;
            }
        }
        
        // tempo de execução desse cara aumenta
        if(ind_atual > -1 ) {
            if(processos[ind_atual].dt <= dt_exec[ind_atual]) {
                if(!pthread_cancel(tid[ind_atual]))
                    printf("a thread foi destruída! c:\n"); // mata a thread
                if(d_option)
                    fprintf(stderr, "Processo %ld acabou e será impresso na linha %d do trace.\n", tid[ind_atual],linhas);
                linhas++;
                int tr = tempo_prog - t0_processos[ind_atual];
                registraProcessos(arq_trace, processos[ind_atual].processo, tempo_prog, tr);
                ind_atual = -1; // acabou o processo
            }
            else
                dt_exec[ind_atual]++;
            
            printf("\n");
        }
        
        // temos a fila, checamos se há necessidade de mudar a posição!
        if(fila) {
            // EXISTE PROCESSO ROLANDO
            if(ind_atual > -1) {
                if( ( fila->estado == Espera && (processos[ind_atual].dt-dt_exec[ind_atual]) > fila->proc.dt )
                    ||
                    ( fila->estado == Dormindo && 
                    ((processos[ind_atual].dt-dt_exec[ind_atual]) > (fila->proc.dt-dt_exec[fila->indice])) )
                    ) { // tempo de exec < q rolando

                    if(fila->estado == Espera)
                        t0_processos[fila->indice] = tempo_prog;


                    Node * novo = (Node *) malloc(sizeof(Node));
                    novo->indice = ind_atual;
                    novo->prox = fila->prox;
                    novo->proc = processos[ind_atual];
                    novo->estado = Dormindo;
                    aux = fila;
                    ind_atual = fila->indice;
                    thread_id_num = tid[ind_atual];
                    fila = novo;
                    free(aux);
                    mud_cont++;
                }
                
                pthread_cond_signal(&c_procs[ind_atual]);
            }
            else { //NÃO EXISTE PROCESSO ROLANDO
                if(fila->estado == Espera)
                    t0_processos[fila->indice] = tempo_prog;
                ind_atual = fila->indice;
                aux = fila;
                fila = fila->prox;
                free(aux);

                tam_fila--;

                thread_id_num = tid[ind_atual];
                pthread_cond_signal(&c_procs[ind_atual]);
            }
        }
        else {
            // continua rodando o atual se tiver
            if(ind_atual > -1) {
                pthread_cond_signal(&c_procs[ind_atual]);
            }
            // caso todos os processos prontos tenham sido lidos, acabou
            else if(ind_prontos >= num_p) {
                terminou = 1;
            }
        }

        printf("\n**********************************************************************\n");
        printf("Tempo do programa: %ld\nÍndice do processo atual (em execução): %d\n", tempo_prog-1, ind_atual);
        printf("Tamanho da fila: %d\n", tam_fila);
        aux = fila;
        printf("FILA: ");
        while(aux) {
            printf("p%d->", aux->indice);
            aux = aux->prox;
        }
        printf("\n");

        tempo_decorrido++;
        tempo_prog++;
        sleep(1);
        
        pthread_mutex_unlock(&m_escalonador);

    }

    /* Esperando todas as threads executarem */
    for (int i = 0; i < num_p; i++)
        if (pthread_join(tid[i], NULL)) {
            printf("\n Erro ao juntar a thread!");
            exit(1);
        }
    
    if(d_option)
        free(thread_c);

    printf("ACABOU!!!!\n");
    if(d_option)
        fprintf(stderr, "%d mudanças de contexto\n", mud_cont);

    registraFim(arq_trace, mud_cont);
    for(int i = 0; i < num_p; i++)
        pthread_mutex_destroy(&m_procs[i]);

    pthread_mutex_destroy(&mutex);
    pthread_mutex_destroy(&m_escalonador);

}


/* FILAS */
//https://www.ime.usp.br/~pf/algoritmos/aulas/lista.html

/* O estado inicial que vai receber é ESPERA e ele é inserido entre p != NULL e o próximo */
void insere(Data d_proc, Node * p) {
    Node * novo;
    novo = (Node *) malloc(sizeof(Node));
    novo->estado = Espera;
    novo->proc = d_proc;
    novo->prox = p->prox;
    p->prox = novo;
}
