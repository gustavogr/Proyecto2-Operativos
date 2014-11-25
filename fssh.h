#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <dirent.h>
#include <signal.h>
#include "comandos.h"

struct hijos
{
	char** nombres;
	int* pipes;
	int n;
};