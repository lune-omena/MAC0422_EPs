

char * nome_arquivo(char * path) {  // Devolve nome do arquivo dado por um path

    char * token = strtok(path, "/");
    char * past;

    while(token) {
        past = token;
        token = strtok(NULL, "/");
    }

    return past;
}

        else if(!strcmp(buf_break, "cp"))
        {
            int i;
            buf_break = strtok(NULL, " "); // pega cp

            for(i = 0; buf_break != NULL && i < 2; i++) { // origem e destino
                args[i] = buf_break;
                buf_break = strtok(NULL, " ");
            }

            args[0]; /* origem */
            args[1]; /* destino */

            char texto[4000];
            char aux[4000];
            char aux2[4000] = "\0";
            int tam_txt = 0;
            int tam_bytes = 0;
            int ini = -1; 

            Diretorio * dir = find_dir(args[1], raiz);

            //if(!access(args[1], F_OK)) { // encontrou path para dest e orig
            if(dir) {

                FILE * f_cp = fopen(args[0], "r");
                char * c_pointer = (char *) malloc(sizeof(char));
                char c_aux;

                if(f_cp) { // encontrou path para arquivo texto

                    printf("Encontrou arquivo! O diretório destino é |%s|!\n", dir->nome);

                    Arquivo * novo_arquivo = (Arquivo *) malloc(sizeof(Arquivo));
                    // preciso atualizar primeiro na FAT e pegar tamanho
                    int pos_ultimo = -1;

                    char c = fgetc(f_cp);


                    while(1) {

                        if( c == EOF || strlen(aux) + 1 == 4000 ) {

                            if(c == EOF)
                                strcpy(c_pointer, aux2);
                            else
                                *c_pointer = c;
                            
                            strcat(aux, c_pointer);

                            int pos = find_bitmap();

                            admin[pos] = (void *) aux;
                            //printf("aqui: |%s|\n", aux);
                            
                            // insere no bitmap/FAT
                            bitmap[pos] = 0;
                            pos_ultimo = pos;

                            //FAT encontra espaço vaGO MUDAR DPS
                            if(pos_ultimo != -1)
                                FAT[pos_ultimo].prox = pos;
                            else
                                ini = pos;

                            FAT[pos].prox = -1;
                            FAT[pos].endereco = admin[pos];
                            atual_bitmap++;

                            tam_bytes += strlen(aux);
                            strcpy(aux, aux2);

                            if(c == EOF)
                                break;

                        }
                        else {
                            *c_pointer = c;
                            strcat(aux, c_pointer);
                        }
                        
                        c = fgetc(f_cp);

                    }

                    // atualizar dados do arquivo dentro do diretório que está
                    novo_arquivo->arq_acesso = novo_arquivo->arq_alterado = novo_arquivo->arq_criado = (unsigned) time(NULL);
                    novo_arquivo->tamanho = (tam_bytes);
                    novo_arquivo->pos_fat = ini;
                    novo_arquivo->nome = nome_arquivo(args[0]);
                    printf("%s criado\n", novo_arquivo->nome);

                    // INSERIR NO diretório

                }
                else 
                    printf("Não existe um arquivo (origem) com esse nome.\n");
            }
            else
                printf("Não existe path destino com esse nome.\n");
            

        }