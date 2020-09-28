FLAGS = -Wall -g -O0 -c

# Para gerar os dois binários, digite no terminal:
# make ep
# 
# Para rodar o binário da shell, digite:
# ./bccsh
#
# Para rodar o escalonador, digite:
# ./ep1 <escalonador> <arquivo_trace> <novo_arquivo> <d>
# sendo <d> opcional!

ep: bccsh ep1

bccsh:  bccsh.o
		gcc bccsh.o -o bccsh -lreadline

bccsh.o: bccsh.c
		gcc $(FLAGS) bccsh.c

ep1: ep1.o
		gcc ep1.o -o ep1 
	
ep1.o: ep1.c
		gcc $(FLAGS) ep1.c