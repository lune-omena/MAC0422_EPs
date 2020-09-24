FLAGS = -Wall -g -O0 -c

# Digite no terminal:
#						1. make bccsh
#					  	2. ./bccsh

bccsh:  bccsh.o
		gcc bccsh.o -o bccsh

bccsh.o: bccsh.c
		 gcc $(FLAGS) bccsh.c