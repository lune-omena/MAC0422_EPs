### Requisitos no enunciado do EP

###### Próximas a serem implementadas

- [ ] Eliminar ultimo ciclista a cada 2 rodadas
- [ ] Registro posição/rank dos ciclistas a cada rodada
- [x] Implementar mudança de velocidade aleatória
- [ ] Ultrapassagem
- [ ] Condição de não aumento de velocidade (acho que a Lara já fez)
- [ ] Quebra de ciclistas
- [ ] Ciclistas largam ordenados aleatoriamente com no maximo 5 lado a lado a cada metro
- [ ] Atualizar velocidade de ciclista amigo que não teve sua velocidade alterada para 90km/h nas 2 ultimas voltas.

---

- [x] criar n threads “ciclista” iguais
- [x] Todos os ciclistas fazem a primeira volta a 30Km/h
- [x] vetor compartilhado “pista” que tem um tamanho igual a d
- [x] a posição i da pista deve possuir os identificadores de todos os ciclistas que estão naquele trecho naquele instante
- [ ] simular a corrida em intervalos de 60ms até as duas últimas voltas. Caso 90km/h...
- [x] Cada thread deve escrever seu identificador na posição correta do vetor pista a cada momento
- [ ] Caso algum ciclista quebre, essa informação deve ser exibida na tela no momento exato em que ele quebrou
- [ ] Destruir threads de ciclistas eliminados ou quebrados
- [ ] Relatório da saída do programa
- [ ] Implementar opção de debug (já meio implementada)
- [x] receber argumentos d e n da linha de comando

### Exibição de resultados

- [ ] Gráficos de barra que facilitem observar qual foi o impacto no uso de memória e no tempo de execução do programa ao aumentar tanto o tempo de simulação
- [ ] Selecionar 3 tamanhos de pista (pequena, média e grande) e 3 quantidades de ciclistas (poucos, normal e muitos)
- [ ] Intervalo de confiança de 30 medições com nı́vel de confiança de 95%
- [ ] Discuta se os resultados saı́ram conforme o esperado

#### Decisões feitas/ dúvidas

- A velocidade do ciclista é mudada a cada rodada individual (não geral do programa, não importando sua classificação)

> Considere ainda que nas 2 últimas voltas há a chance de 10% de 1 ciclista aleatório qualquer e respeitando as regras de ultrapassagem, fazer essas 2 últimas voltas a 90Km/h

- Todo ciclista na ultima volta tem essa probabilidade. Em casos extremos, como nas ultimas duas voltas haverão dois ciclistas, ambos tem a chance de completar a volta a 90km/k. Isso é um grande problema? Está ruim assim?

- Para indicar que uma posição se encontra vazia na pista, indicamos com 0

- Pista é uma matriz de tipo pthread_t **

