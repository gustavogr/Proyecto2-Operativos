all: comandos.o
	gcc -g -Wall comandos.o fssh.c -o fssh
	rm *.o
comandos.o:
	gcc -g -Wall -c comandos.c
