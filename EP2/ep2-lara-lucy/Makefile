FLAGS = -Wall -g -O0 -c
# Para rodar, digite no terminal
# make ep2

ep2: ep2.o
	gcc ep2.o -pthread -o ep2

ep2.o: ep2.c ep2.h
	gcc $(FLAGS) ep2.c