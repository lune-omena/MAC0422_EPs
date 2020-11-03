### Requisitos no enunciado do EP

###### Próximas a serem implementadas

- [ ] Resolver bug de último colocado não ser colocado no ranking geral (caso sem quebras) (em desenvolvimento)
- [ ] Resolver bug de rankings de rodadas serem "Pulados" (caso com >2 quebras e >10 rodadas)
- [ ] simular a corrida em intervalos de 60ms até as duas últimas voltas. Caso 90km/h...
- [ ] Atualizar velocidade de ciclista amigo que não teve sua velocidade alterada para 90km/h nas 2 ultimas voltas.
- [ ] Implementar opção de debug (já meio implementada) que informa a cada 60ms (e 20ms nas duas últimas voltas, caso alguém pedale a 90Km/h) o status de cada posição da pista, ou seja, o identificador do(s) ciclista(s) naquela posição ou a informação de que não há nenhum ciclista ali.
- [ ] Inserir valores de consumo de memória! (ai)
- [x] Relatório da saída do programa informando a cada volta completada, as posições de todos os ciclistas naquela volta
- [x] Inserir relógio no programa
- [x] Ajustar condições de corrida
- [x] Inserir identificador para cada thread
- [x] Registro posição/rank dos ciclistas a cada rodada
- [x] Quebra de ciclistas
- [x] Caso algum ciclista quebre, essa informação deve ser exibida na tela no momento exato em que ele quebrou
- [x] Ultrapassagem
- [x] Implementar mudança de velocidade aleatória
- [x] Eliminar ultimo ciclista a cada 2 rodadas
- [x] Condição de não aumento de velocidade
- [x] Corrigir condições de iteração do programa (até quando irá rodar, determinação de volta/rodada etc.)
- [x] criar n threads “ciclista” iguais
- [x] Ciclistas largam ordenados aleatoriamente com no maximo 5 lado a lado a cada metro
- [x] Todos os ciclistas fazem a primeira volta a 30Km/h
- [x] vetor compartilhado “pista” que tem um tamanho igual a d
- [x] a posição i da pista deve possuir os identificadores de todos os ciclistas que estão naquele trecho naquele instante
- [x] Cada thread deve escrever seu identificador na posição correta do vetor pista a cada momento
- [x] Destruir threads de ciclistas eliminados ou quebrados
- [x] receber argumentos d e n da linha de comando

### Exibição de resultados

- [ ] Selecionar 3 tamanhos de pista (pequena, média e grande) e 3 quantidades de ciclistas (poucos, normal e muitos) -> (250, 500, 750) e (250, 500, 750) ?
- [ ] Gráficos de barra que facilitem observar qual foi o impacto no uso de memória e no tempo de execução do programa ao aumentar tanto o tempo de simulação quanto o nro de threads
- [ ] Intervalo de confiança de 30 medições com nı́vel de confiança de 95%
- [ ] Discuta se os resultados saı́ram conforme o esperado

#### Decisões feitas/ dúvidas

- A velocidade do ciclista é mudada a cada rodada individual (não geral do programa, não importando sua classificação)

> Considere ainda que nas 2 últimas voltas há a chance de 10% de 1 ciclista aleatório qualquer e respeitando as regras de ultrapassagem, fazer essas 2 últimas voltas a 90Km/h

- Todo ciclista na ultima volta tem essa probabilidade. Em casos extremos, como nas ultimas duas voltas haverão dois ciclistas, ambos tem a chance de completar a volta a 90km/k. Isso é um grande problema? Está ruim assim? não acho problemático
- Para indicar que uma posição se encontra vazia na pista, indicamos com 0
- Pista é uma matriz de tipo pthread_t **
- MUDANÇA NA IMPLEMENTAÇÃO: rodadas começam a partir de 1, e não 0 (visto 0 representa threads já finalizadas)
- A eliminação dos ciclistas é feita de 2 em 2 voltas - isso é feito a partir de uma lista ligada em que registramos a classificação de cada ciclista associada a cada rodada (portanto, guardamos em cada célula da lista ligada o registro da classificação da rodada)
- no momento em que a célula não tem mais uso, eu descarto ela e assumo uma nova cabeça para a lista ligada (a próxima célula)
-  decidimos que a aleatoriedade no sorteio da thread a ser eliminada (caso esteja na mesma rodada e mesma posição final na mesma velocidade que outras tantas threads) se dá pela escolha do escalonador
-  Quando o primeiro ciclista completar a quantidade ideal de voltas na pista, ele é retirado de pódio e tem sua posição registrada na volta indicada.
- a quebra do ciclista é determinada na última posição da volta anterior à volta%6, visto que dessa forma há possibilidade de se organizar a quantidade de ciclistas que quebraram naquele turno pelo escalonador. Isso impossibilita a ocorrência de condições de corrida pois a atualização de ciclistas quebrados pra cada rodada é feita na área restrita à thread do escalonador