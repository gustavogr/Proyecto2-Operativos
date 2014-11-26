#include <string.h>
#include <stdio.h>
#include <sys/wait.h>
#include "comandos.h"

struct hijos
{
	char** nombres;
	int* pipes;
	int n;
};

void liberarHijos(struct hijos *);

struct hijos * procesarHijo(struct hijos *);

struct hijos * procesarRaiz(struct hijos *,int);