all: comandos.o
	gcc -g -Wall comandos.o fssh.c -o fssh
comandos.o:
	gcc -g -Wall -c comandos.c