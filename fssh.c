#include "fssh.h"

struct hijos * generarHijos()
{
	int numsubdir,fd[2];
	struct dirent *entradaDir;
	DIR *dirp;
	struct hijos * h = malloc(sizeof(struct hijos));
	struct stat statbuf;
	numsubdir = 0;

	if ((dirp = opendir(".")) == NULL) perror("generarHijos-- opendir");
	while ((entradaDir = readdir(dirp)) != NULL)
	{
		if (strcmp(entradaDir->d_name,".") != 0 && strcmp(entradaDir->d_name,"..") != 0)
		{
			if (stat(entradaDir->d_name,&statbuf) == 1)
			{
				fprintf(stderr, "No se pudo aplicar stat sobre %s: %s \n", 
											entradaDir->d_name, strerror(errno));
				exit(1);	
			}
			if (statbuf.st_mode & S_IFDIR)
			{
				numsubdir++;
			}
		}
	}	
	closedir(dirp);
	if (numsubdir == 0)
	{
		return (struct hijos *) NULL;
	}
	dirp = opendir(".");
	h->nombres = malloc(numsubdir * sizeof(char*));
	h->pipes = malloc(numsubdir * sizeof(int));
	h->n=numsubdir;
	numsubdir = 0;
	while ((entradaDir = readdir(dirp)) != NULL)
	{
		if (strcmp(entradaDir->d_name,".") != 0 && strcmp(entradaDir->d_name,"..") != 0)
		{
			stat(entradaDir->d_name,&statbuf);
			if (statbuf.st_mode & S_IFDIR)
			{
				pipe(fd);
				if (fork() == 0) // Codigo hijo
				{
					close(fd[1]);
					dup2(fd[0],0);
					close(fd[0]);
					chdir(entradaDir->d_name);
					struct hijos *haux = generarHijos();
					int i;
					char* instruccion = malloc(100);
					read(0,instruccion,1);
					if (strcmp(instruccion,"S") == 0)
					{
						printf("Señal %s recibida por %s\n",instruccion,entradaDir->d_name);
						if (haux != NULL)
						{
							for (i = 0; i < haux->n; ++i)
							{
								write(haux->pipes[i],"S",1);
							}
						}
					}
					printf("Soy el hijo %s", entradaDir->d_name);
					exit(1);
					
				} else {
					close(fd[0]);
					h->nombres[numsubdir] = malloc(strlen((entradaDir->d_name)+1)*(sizeof(char)));
					strcpy(h->nombres[numsubdir],entradaDir->d_name);
					h->pipes[numsubdir] = fd[1];
					numsubdir++;
				}

			}
		}
	}




	return h;
}


int main(int argc, char const *argv[])
{
	struct stat statbuf;
	// Deteccion de Errores de entrada
	if (argc != 2)
	{
		printf("Uso: fssh DirectorioRaiz.\n");
		exit(1);
	}
	if (stat(argv[1],&statbuf) == 1) {
		fprintf(stderr, "No se pudo aplicar stat sobre el archivo %s: %s \n", argv[1], strerror(errno));
		exit(1);
	}
	if (!(statbuf.st_mode & S_IFDIR)) {
		printf("%s no es un directorio. No se puede iniciar el shell.\n", argv[1]);
		exit(1);
	}

	// El proceso raiz se mueve al directorio especificado
	chdir(argv[1]);
	int salidaEstandar, lectura, fd[2];

	// Guardo salida estandar
	salidaEstandar = dup(1);

	// Creamos el pipe de subida y lo ponemos en salida estandar para los procesos hijos
	pipe(fd);
	printf("A comenzar.\n");
	dup2(fd[1],1);
	close(fd[1]);
	lectura = fd[0];
	struct hijos *h = generarHijos();
	dup2(salidaEstandar,1);
	int i;
	for (i = 0; i < h->n; ++i)
	{
		printf("Hijo %s, pipe %d.\n",h->nombres[i],h->pipes[i]);
		write(h->pipes[i],"S",1);

	}
	for (i = 0; i < 10; ++i)
	{
		char * buffer = malloc(200);
		read(lectura,buffer,200);
		printf("%s\n",buffer);
	}

	return 0;
}