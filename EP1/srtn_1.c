
void * thread_srtn(void *a)
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

        /* if(sinal) {
            pausa thread
            break?
            }*/

        /* PROTOCOLO DE SAIDA */
        pthread_mutex_unlock(&mutex); // V() -> incrementa após P()
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
    int existe = 0;                  // indica se existe thread em execução

    int rodando = 0;

    /* FILA */
    Node * fila = NULL; // fila de processos prontos
    Node * aux; // iteração
    Node * ant; // iteração

    pthread_mutex_init(&mutex, NULL);

    int terminou = 0;
    while(!terminou/* ainda não foram todos os processos*/) {

        /* checa os processos prontos a serem adicionados à fila */
        while(tempo_prog > processos[ind_prontos].d0) {
            
            if(!fila) { // fila vazia
                fila->proc = processos[ind_prontos];
                fila->prox = NULL;
            }
            else {
                aux = fila;
                ant = NULL;

                while(aux) {
                    if(processos[ind_prontos].dt < aux->proc.dt) {
                        if(ant)
                            insere(processos[ind_prontos], ant);
                        else { // primeiro termo
                            Node * novo = (Node *) malloc(sizeof(Node));
                            novo->proc = processos[ind_prontos];
                            novo->prox = fila;
                            fila = novo;
                        }
                        break;
                    }

                    ant = aux;
                    aux = aux->prox;
                }
                
                if(!aux && ant != NULL) { // último termo
                    Node * novo = (Node *) malloc(sizeof(Node));
                    novo->proc = processos[ind_prontos];
                    novo->prox = NULL;
                    ant->prox = novo;
                }
            }

            ind_prontos++;
        }


        /* checa se o processo em execução morreu */
        if(tempo_dt - tempo_decorrido <= 0) {
            existe = 0;
            // pega o indice dele
            // destroi 
        }

        /* executa */
        /* CASO 1: existe thread rodando */
        if(existe) {
            //vai ter que trocar
            if(fila->proc.dt < tempo_dt - tempo_decorrido) {
                // pausa o atual
                // coloca como estado dormindo e atualiza dt
                if(fila->estado == Espera) {
                    tempo_dt = fila->proc.dt;
                    if (pthread_create(&tid[ind], NULL, thread, NULL)) {
                        printf("\n ERROR creating thread\n");
                        exit(1);
                    }
                    ind++;
                }
                else if(fila->estado == Dormindo) {

                }
                // pusha o anterior pro início da fila
            }
        }
        /* CASO 2: não existe thread rodando */
        else if(fila != NULL){ 
            if(fila->estado == Espera) {
                tempo_dt = fila->proc.dt;
                aux = fila;
                fila = fila->prox;
                free(aux);
                if (pthread_create(&tid[ind], NULL, thread, NULL)) {
                    printf("\n ERROR creating thread\n");
                    exit(1);
                }
                ind++;
                existe = 1;
            }
            else if(fila->estado == Dormindo) {
                // vai ter que reativar a thread
                // vejo isso amanhã
            }
            else {
                printf("Você não devia ter chegado aqui...\n");
            }
        }
        else if(fila == NULL) {
            // vai pro próximo tempo
            sleep(1);
            tempo_prog++;

            //adicionar caso pra terminar o programa
        }

    }

    /* Esperando todas as threads executarem */
    // não sei se é necessário fazer o join...?aaaas
    for (int i = 0; i < num_p; i++)
        if (pthread_join(tid[i], NULL)) {
            printf("\n Erro ao juntar a thread!");
            exit(1);
        }

    pthread_mutex_destroy(&mutex);

}
