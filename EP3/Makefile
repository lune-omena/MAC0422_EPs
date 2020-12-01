FLAGS = -Wall -g -O0 -c

# Para gerar os dois binários, digite no terminal:
# make ep
# 
# Para rodar o binário da shell, digite:
# ./ep3
#
# Para rodar o escalonador, digite:
# ./ep1 <escalonador> <arquivo_trace> <novo_arquivo> <d>
# sendo <d> opcional!
#
# (também podemos rodar ./ep1 pelo ep3)

ep: ep3 

ep3:  ep3.o
		gcc ep3.o -o ep3 -lreadline

ep3.o: ep3.c ep3.h
		gcc $(FLAGS) ep3.c

teste: teste.o
		gcc teste.o -o teste -lpthread -pthread

teste.o: teste.c
		gcc $(FLAGS) teste.c -lpthread -pthread
