#include "fssh.h"

void sigPipeHandler()
{
	printf("Se murio un pipe..\n");
}

void liberarHijos(struct hijos * h){
	int i;
	for (i = 0; i < h->n; ++i)
	{
		free(h->nombres[i]);
		close(h->pipes[i]);
	}
	free(h->nombres);
	free(h->pipes);
	free(h);
}

struct hijos * generarHijos()
{
	int numsubdir,fd[2];
	struct dirent *entradaDir;
	DIR *dirp;
	
	struct stat statbuf;
	numsubdir = 0;

	if ((dirp = opendir(".")) == NULL) perror("generarHijos-- opendir");
	while ((entradaDir = readdir(dirp)) != NULL)
	{
		if (strcmp(entradaDir->d_name,".") != 0 && strcmp(entradaDir->d_name,"..") != 0)
		{
			if (stat(entradaDir->d_name,&statbuf) == -1)
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
	struct hijos * h = malloc(sizeof(struct hijos));
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
					// Eliminamos el h del padre
					int i;
					for (i = 0; i < numsubdir; ++i)
					{
						free(h->nombres[i]);
						close(h->pipes[i]);
					}
					free(h->nombres);
					free(h->pipes);
					free(h);

					chdir(entradaDir->d_name);
					closedir(dirp);
					struct hijos *haux = generarHijos();
					





					if (haux != NULL) liberarHijos(haux);
					exit(1);	
				} else {  // Codigo padre
					close(fd[0]);
					h->nombres[numsubdir] = malloc((strlen(entradaDir->d_name)+1)*(sizeof(char)));
					strcpy(h->nombres[numsubdir],entradaDir->d_name);
					h->pipes[numsubdir] = fd[1];
					numsubdir++;
				}
			}
		}
	}
	closedir(dirp);
	return h;
}

void procesarRaiz(struct hijos * h,int resultado)
{
	char * comando = malloc(7*sizeof(char));
	char * path1 = malloc(256*sizeof(char));
	char * path2 = malloc(256*sizeof(char));
	char * buffer = malloc(520*sizeof(char));
	char * instruccion = malloc(520*sizeof(char));
	while(1)
	{
		printf("fssh> ");
		fgets(instruccion,519,stdin);
		if (strlen(instruccion)>0) instruccion[strlen(instruccion)-1] = '\0';
		strcpy(buffer,instruccion);

		comando = strtok(buffer," ");
		path1 = strtok(NULL," ");
		path2 = strtok(NULL," ");
		if (strcmp(comando,"ls") == 0)
		{
			if (path1 == NULL || path2 != NULL)
			{
				printf("Uso: ls <path1>\n");
				continue;
			}
			if (strcmp(path1,"/") == 0)
			{
				ls(".");
				continue;
			}
			char * obj = malloc(30*sizeof(char));
			char * sig = malloc(30*sizeof(char));
			obj = strtok(path1,"/");
			sig = strtok(NULL,"/");
			int i;
			for (i = 0; i <  h-> n; ++i)
			{
				/* code */
			}



		} 
		else if (strcmp(comando,"cat") == 0)
		{
			/* code */
		} 
		else if (strcmp(comando,"cp") == 0)
		{
			/* code */
		} 
		else if (strcmp(comando,"mv") == 0)
		{
			/* code */
		} 
		else if (strcmp(comando,"find") == 0)
		{
			/* code */
		} 
		else if (strcmp(comando,"rm") == 0)
		{
			/* code */
		} 
		else if (strcmp(comando,"mkdir") == 0)
		{
			/* code */
		} 
		else if (strcmp(comando,"rmdir") == 0)
		{
			/* code */
		} 
		else if (strcmp(comando,"quit") == 0)
		{
			/* code */
		} else {
			printf("Comando invalido.\n");
		}

	}
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
	signal(SIGPIPE,sigPipeHandler);
	// Guardo salida estandar
	salidaEstandar = dup(1);

	// Creamos el pipe de subida y lo ponemos en salida estandar para los procesos hijos
	pipe(fd);
	dup2(fd[1],1);
	close(fd[1]);
	lectura = fd[0];
	struct hijos *h = generarHijos();
	dup2(salidaEstandar,1);
	close(salidaEstandar);
	procesarRaiz(h,lectura);


	liberarHijos(h);
	return 0;
}