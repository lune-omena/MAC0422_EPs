
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
            }*/

        /* PROTOCOLO DE SAIDA */
        pthread_mutex_unlock(&mutex); // V() -> incrementa após P()
    }

    return NULL;
}

void SRTN(Data * processos, int num_p) {
    /* Ordena os processos prontos em uma fila por ordem do tempo de execução deles. 
     * Do mais curto para o mais longo e executa nessa ordem */
    /* O tempo de execução dele é comparado com o tempo que falta do processo que
     * está sendo executado. Se o novo processo é mais curto, ele passa a executar 
     * e o atual vai pra fila de prontos para continuar sua execução depois */

    int ind = 0;                     // índice do vetor de processos
    pthread_t tid[num_p];            // vetor de threads
    tempo_prog = 0;                  // tempo decorrido do programa
    int terminou = 0;
    int existe = 0;                  // indica se existe thread em execução

    /* FILA:
    int ind = 0;                     // índice no vetor processos de processos lidos
    int ind_prontos = 0;             // número de threads criadas
    pthread_t tid[num_p];            // vetor de threads
    tempo_prog = 0;                  // tempo decorrido do programa
    int terminou = 0;
    int existe = 0;                  // indica se existe thread em execução

    int ind_prontos = 0;             // número de threads criadas
    int i;
    Data * fila = (Data *) malloc(num_p*sizeof(Data));
    int front = 0;
    int rear = -1;
    int num_prontos = 0;*/

    /* LISTA LIGADA */
    Node * fila = NULL;
    Node * atual = NULL;

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

        /* Preciso rodar este loop de forma que ele checa com o tempo
         * talvez de um segundo em um segundo */

        if(!existe) {
            if(fila) {
                if(tempo_prog < fila->proc.d0) {
                    tempo_prog++;
                    sleep(1);
                }
                else {// preciso pensar no caso de ser o primeiro, aí a fila nem foi criada ainda...
                    tempo_dt = fila->proc.dt;
                    tempo_decorrido = 0;

                    if (pthread_create(&tid[ind], NULL, thread, NULL)) {
                        printf("\n ERROR creating thread\n");
                        exit(1);
                    }
                    else {
                        ind++;
                        existe = 1;
                    }

                }
            }
        }

        // PEGA TODOS OS PROCESSOS DE T0 == TEMPO_PROG
        while(tempo_prog >= processos[ind].d0) {
            // adiciona na fila 
            // atualiza o índice do último item da fila;
            if( fila == NULL ) { // fila vazia
                // checa se o dt é menor que o do atual
                if(!existe) {
                    if (pthread_create(&tid[ind], NULL, thread, NULL)) {
                        printf("\n ERROR creating thread\n");
                        exit(1);
                    }
                    ind++;
                    existe = 1;
                }
                else {
                    if(processos[ind].dt < tempo_dt - tempo_decorrido) {
                        // checo se a thread desse processo já existe TAMBÉM

                        //PAUSAR THREAD DO OUTRO E RESUMIR COM ESSA.
                        if (pthread_create(&tid[ind], NULL, thread, NULL)) {
                            printf("\n ERROR creating thread\n");
                            exit(1);
                        }
                        else 
                            ind++;
                    }
                    else { //inicia fila
                        fila->proc = processos[ind];
                        fila->next = NULL;
                        atual = fila;
                    }
                }
            }
            // FILA NÃO VAZIA:
            else {
                // CHECA SE DT É MENOR QUE DO ATUAL QUE ESTÁ RODANDO 
                if( (processos[ind].dt < tempo_dt - tempo_decorrido) && existe) {
                    // adiciona processo no começo da fila, ou seja,
                    // quando for botar de volta na fila, vou colocar na posição [front-1]
                    // pq n tem problema já que necessáriamente vai ser um número > 0    
                }
                // SE NÃO FOR, COMPARA COM O RESTO DA FILA
                else {
                    Node *itr = fila;
                    Node * ant = NULL;
                    Node * novo = (Node *) malloc(sizeof(Node));
                    novo->proc = processos[ind];

                    while(itr != NULL) {

                        if(processos[ind].dt < itr->proc.dt) {
                            /* cria o processo */
                            novo->next = itr;

                            if(ant != NULL) {
                                ant->next = novo;
                            }
                            else
                                fila = novo;
                            
                            break;
                        }
                        
                        ant = itr;
                        itr = itr->next;
                    }
                    // ultimo e n tem menor
                    if(itr == NULL) {
                        ant->next = novo;
                        atual = novo;
                    }
                }
            }
            //ind++;
        }

        //ind++;

        /* CASO NÃO HAJAM PROGRAMAS EM EXECUÇÃO: */
        /*
        if(!existe) {
            if(tempo_prog < fila[front].d0) {
                tempo_prog++;
                sleep(1);
            }
            else {// preciso pensar no caso de ser o primeiro, aí a fila nem foi criada ainda...
                tempo_dt = fila[front].dt;
                tempo_decorrido = 0;

                if (pthread_create(&tid[ind_prontos], NULL, thread, NULL)) {
                    printf("\n ERROR creating thread\n");
                    exit(1);
                }
                else {
                    front++;
                    ind_prontos++;
                    existe = 1;
                }
            }
        }

        // PEGA TODOS OS PROCESSOS DE T0 == TEMPO_PROG
        while(tempo_prog >= processos[ind].d0) {
            // adiciona na fila 
            // atualiza o índice do último item da fila;
            if( rear == -1 ) { // fila vazia
                // checa se o dt é menor que o do atual
                if(!existe) {
                    if (pthread_create(&tid[ind_prontos], NULL, thread, NULL)) {
                        printf("\n ERROR creating thread\n");
                        exit(1);
                    }
                    else {
                        ind_prontos++;
                        existe = 1;
                    }
                }
                else {
                    if(processos[ind].dt < tempo_dt - tempo_decorrido) {

                        // checo se a thread desse processo já existe
                        if (pthread_create(&tid[ind_prontos], NULL, thread, NULL)) {
                            printf("\n ERROR creating thread\n");
                            exit(1);
                        }
                        else 
                            ind_prontos++;
                    }
                    else
                        // SE NÃO FOR, BOTA NA FILA 
                        fila[++rear] = processos[ind];
                }
            }
            else {
                // CHECA SE DT É MENOR QUE DO ATUAL QUE ESTÁ RODANDO 
                if( (processos[ind].dt < tempo_dt - tempo_decorrido) && existe) {
                    // adiciona processo no começo da fila, ou seja,
                    // quando for botar de volta na fila, vou colocar na posição [front-1]
                    // pq n tem problema já que necessáriamente vai ser um número > 0    
                }
                // SE NÃO FOR, COMPARA COM O RESTO DA FILA
                else {
                    for(i = front; i < rear && processos[ind].dt < fila[i].dt; i++);
                    if(processos[ind].dt < fila[i].dt) {
                        fila[++rear] = fila[i];
                        fila[i] = processos[ind];
                    }
                }
            }
            ind++;
        }*/

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