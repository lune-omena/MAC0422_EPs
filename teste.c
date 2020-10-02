/* Baseado em código de 
 * http://www.csc.villanova.edu/~mdamian/threads/posixsem.html 
 * e no tutorial em e no tutorial em https://computing.llnl.gov/tutorials/pthreads/#Mutexes */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define NITER 1000000000
#define VEZES 5

long int count1=0;
long int count2=0;

pthread_mutex_t mutex1;
pthread_mutex_t mutex2;

// Conta o número de linhas do arquivo
int  contaLinhas(char * arquivo);
// Captação de texto em arquivo e armazenamento
void armazenaProcessos(char * arquivo, Data * processos);


/* O processo implementado utiliza o mutex para garantir quem
   acessa o count1 seja uma thread por vez */
void * ThreadAdd1 (void * a) {
    long int i;
    
    for (i=0; i<(long int)NITER; i++) {
        pthread_mutex_lock(&mutex1);
        count1++;
        pthread_mutex_unlock(&mutex1);
    }

    return NULL;
}

void * ThreadAdd2 (void * a) {
    long int i;
    
    for (i=0; i<(long int)NITER; i++) {
        pthread_mutex_lock(&mutex2);
        count2++;
        pthread_mutex_unlock(&mutex2);
    }

    return NULL;
}

void * existe(void * a)
{
    /* quero receber a duração da thread*/
    /* preciso receber o inicio dela e retornar o final? talvez sim, talvez não o.o*/
    int duracao = (int) a; /* convertendo parametro de entrada */
    int i;

    /* PROTOCOLO DE ENTRADA*/   
    /* verificando se thread pode acessar região crítica */
    V(); 

    /* REGIÃO CRITICA - EXECUTAR O PROCESSO POR X SEGUNDOS */   
    for(i = 0; i < duracao; i++)
    {
        i++;
        printf("Rodando processo por %d segundos...", i);
        sleep(1);
    }

    /* PROTOCOLO DE SAIDA,   libera outra thread */
    P();

    return NULL;
}


int main() {
    int i;
    clock_t inicio, atual;
    double decorrido = 0;
    int num_p;                      // número de processos do arquivo
    Data * processos; 

    if(argv[2] == NULL) {
        printf("Você precisa inserir um arquivo txt como segundo parâmetro!\n");
        exit(EXIT_FAILURE);
    }

    num_p = contaLinhas(argv[2]);
    processos = (Data *) malloc(num_p*sizeof(Data));
    armazenaProcessos(argv[2], processos);

    printf("\nProcessos armazenados:\n");
    for(int i = 0; i < num_p; i++) 
        printf("%s %d %d %d\n", processos[i].processo, processos[i].d0, processos[i].dt, processos[i].deadline);

    printf("\n");


    /* criando threads */
    pthread_t *tid;

    start = clock();

    tid = (pthread_t *) malloc( num_p * sizeof(pthread_t));
    
    for (i = 0; i < num_p; i++)
    {
        if (pthread_create(&tid[i], NULL, existe, NULL))
        {
            printf("\n Erro ao criar a thread %d!", i);
        }
    }

    
    /* Esperando todas as threads executarem */
    for (i = 0; i < num_p; i++)   
        if (pthread_join(tid[i], NULL)) 
        {
            printf("\n Erro ao juntar a thread!");
            exit(1);
        }





/*     pthread_t tid[2*VEZES]; */

    /* iniciando/criando threads com configuração padrão(NULL)*/
/*     pthread_mutex_init(&mutex1, NULL);
    pthread_mutex_init(&mutex2, NULL); */
  
    /* Lançando as threads */
/*     for (i=0; i<VEZES; i++) {
        if (pthread_create(&tid[i], NULL, ThreadAdd1, NULL)) {
            printf("\n ERROR creating thread 1");
            exit(1);
        }

        /* Caso queiramos levar algo para a função, um parâmetro, o ultimo valor não será nulo */
      /*  if (pthread_create(&tid[i+VEZES], NULL, ThreadAdd2, NULL)) {
            printf("\n ERROR creating thread 2");
            exit(1);
        }
    } */
    
    /* "Juntando" as threads uma a uma */
/*     for (i=0; i<2*VEZES; i++)   
        if (pthread_join(tid[i], NULL))  {
            printf("\n ERROR joining thread");
            exit(1);
        } */

    /* Averiguando resultados obtidos */
/*     if (count1<(long int)VEZES*NITER) 
        printf("\n OXE! a variavel count1 eh [%li], mas devia ser %li\n", count1, (long int) VEZES*NITER);
    else
        printf("\n Ufa! a variavel count1 eh [%li]\n", count1);

    if (count2<(long int)VEZES*NITER) 
        printf("\n OXE! a variavel count2 eh [%li], mas devia ser %li\n", count2, (long int) VEZES*NITER);
    else
        printf("\n Ufa! a variavel count2 eh [%li]\n", count2);

    pthread_mutex_destroy(&mutex1);
    pthread_mutex_destroy(&mutex2); */

    return(0);
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
                    size = strlen(buf_break);
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
