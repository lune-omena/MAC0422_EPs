### Requisitos no enunciado do EP

- [ ] Eliminar ultimo ciclista a cada 2 rodadas
- [ ] Registro posição dos ciclistas a cada rodada
- [x] Implementar mudança de velocidade aleatória
- [ ] Ultrapassagem
- [ ] Quebra de ciclistas
- [ ] Ciclistas largam ordenados aleatoriamente com no maximo 5 a lado a lado a cada metro
- [ ] Atualizar velocidade de ciclista amigo que não teve sua velocidade alterada para 90km/h.



#### Decisões feitas

- A velocidade do ciclista é mudada a cada rodada individual (não geral do programa, não importando sua classificação)

> Considere ainda que nas 2 últimas voltas há a chance de 10% de 1 ciclista aleatório qualquer e respeitando as regras de ultrapassagem, fazer essas 2 últimas voltas a 90Km/h

- Todo ciclista na ultima volta tem essa probabilidade. Em casos extremos, como nas ultimas duas voltas haverão dois ciclistas, ambos tem a chance de completar a volta a 90km/k. Isso é um grande problema? Está ruim assim?