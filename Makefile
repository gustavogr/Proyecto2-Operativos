all: comandos.o procesarComandos.o fssh.o
	gcc -g -Wall comandos.o procesarComandos.o fssh.o -o fssh
	rm *.o
.c.o:
	gcc -g -Wall -c $<
