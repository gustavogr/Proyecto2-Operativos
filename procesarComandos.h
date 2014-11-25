#include <string.h>
#include <stdio.h>
#include "comandos.h"

struct hijos
{
	char** nombres;
	int* pipes;
	int n;
};

void liberarHijos(struct hijos *);

void procesarHijo(struct hijos *);

void procesarRaiz(struct hijos *,int);