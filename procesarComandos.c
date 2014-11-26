#include "procesarComandos.h"

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


struct hijos * procesarHijo(struct hijos * h)
{
	char * comando = NULL;
	char * path1 = NULL;
	char * buffer = malloc(520*sizeof(char));
	char * instruccion = malloc(520*sizeof(char));
	while(1)
	{
		read(0,instruccion,520);
		strcpy(buffer,instruccion);
		comando = strtok(buffer," ");
		path1 = strtok(NULL," ");
		if (strcmp(comando,"ls") == 0)
		{
			if (strcmp(path1,"/") == 0)
			{
				ls(".");
				continue;
			}
			char * obj = NULL;
			char * sig = NULL;
			obj = strtok(path1,"/");
			int i;
			int escrito = 0;
			if (h != NULL)
			{
				for (i = 0; i <  h-> n; ++i)
				{
					if (strcmp(h->nombres[i],obj) == 0)
					{
						memset(instruccion,0,520);
						strcpy(instruccion,"ls /");
						while( (sig=strtok(NULL,"/")) != NULL)
						{
							strcat(instruccion,sig);
							strcat(instruccion,"/");
						}
						write(h->pipes[i],instruccion,520);
						escrito = 1;
					}
				}
			}
			if (!escrito)
			{
				if (obj == NULL) write(1,"Path invalido\n",20);
				else {
					ls(obj);
				}
			}
		} 
		else if (strcmp(comando,"cat") == 0)
		{	
			char * obj = NULL;
			char * sig = NULL;
			obj = strtok(path1,"/");
			sig = strtok(NULL,"/");
			if (sig == NULL)
			{
				cat(obj);
				continue;
			}

			int i;
			int escrito = 0;
			if (h != NULL)
			{
				for (i = 0; i <  h-> n; ++i)
				{
					if (strcmp(h->nombres[i],obj) == 0)
					{
						memset(instruccion,0,520);
						strcpy(instruccion,"cat /");
						strcat(instruccion,sig);
						while( (sig=strtok(NULL,"/")) != NULL)
						{
							strcat(instruccion,"/");
							strcat(instruccion,sig);
						}
						write(h->pipes[i],instruccion,520);
						escrito = 1;
					}
				}
			}	
			if (!escrito)
			{
				printf("Path invalido.\n");				
			}
		}  
		else if (strcmp(comando,"rm") == 0)
		{
			char * obj = NULL;
			char * sig = NULL;
			obj = strtok(path1,"/");
			sig = strtok(NULL,"/");
			if (sig == NULL)
			{
				rm(obj);
				write(1,"",2);
				continue;
			}

			int i;
			int escrito = 0;
			if (h != NULL)
			{
				for (i = 0; i <  h-> n; ++i)
				{
					if (strcmp(h->nombres[i],obj) == 0)
					{
						memset(instruccion,0,520);
						strcpy(instruccion,"rm /");
						strcat(instruccion,sig);
						while( (sig=strtok(NULL,"/")) != NULL)
						{
							strcat(instruccion,"/");
							strcat(instruccion,sig);
						}
						write(h->pipes[i],instruccion,520);
						escrito = 1;
					}
				}
			}
			if (!escrito)
			{
				printf("Path invalido.\n");				
			}
		} 
		else if (strcmp(comando,"mkdir") == 0)
		{
			char * obj = NULL;
			char * sig = NULL;
			obj = strtok(path1,"/");
			sig = strtok(NULL,"/");
			if (sig == NULL)
			{
				if (mkdir(obj,0777) == -1)
				{
					if (errno == 17)
					{
						printf("Ya existe un archivo con nombre %s.\n", obj);
						continue;
					}
					else
					{
						printf("Error en mkdir: %s\n",strerror(errno));
						continue;
					}
				} 
				else
				{
					write(1,"",2);
					int fd[2];
					pipe(fd);
					if (fork() == 0)
					{
						close(fd[1]);
						dup2(fd[0],0);
						close(fd[0]);
						chdir(obj);
						//liberarHijos(h);
						h = procesarHijo(NULL);
						// Cuando salga de la funcion esta terminando.
						free(instruccion);
						free(buffer);
						return h;
					}
					else
					{
						close(fd[0]);
						if (h != NULL)
						{
							h->nombres = realloc(h->nombres,(h->n+1)*sizeof(char*));
							h->pipes = realloc(h->pipes,(h->n+1)*sizeof(int));
							h->nombres[h->n] = malloc(strlen(obj)*sizeof(char));
							strcpy(h->nombres[h->n],obj);
							h->pipes[h->n] = fd[1]; 
							h->n ++;
						}
						else {
							h = malloc(sizeof(struct hijos));
							h->nombres = malloc(1 * sizeof(char*));
							h->pipes = malloc(1 * sizeof(int));
							h->n=1;
							h->nombres[0] = malloc(strlen(obj)*sizeof(char));
							strcpy(h->nombres[0],obj);
							h->pipes[0] = fd[1];
						}
						continue;
					}
				}
			}
			int i;
			int escrito = 0;
			if (h != NULL)
			{	
				for (i = 0; i <  h-> n; ++i)
				{
					if (strcmp(h->nombres[i],obj) == 0)
					{
						memset(instruccion,0,520);
						strcpy(instruccion,"mkdir /");
						strcat(instruccion,sig);
						while( (sig=strtok(NULL,"/")) != NULL)
						{
							strcat(instruccion,"/");
							strcat(instruccion,sig);
						}
						write(h->pipes[i],instruccion,520);
						escrito = 1;
					}
				}
			}
			if (!escrito)
			{
				printf("Path invalido.\n");				
			}
		} 
		else if (strcmp(comando,"quit") == 0)
		{
			if (h != NULL)
			{
				int i;
				for (i = 0; i < h->n; ++i)
				{
					write(h->pipes[i],"quit",10);
				}
				for (i = 0; i < h->n; ++i)
				{
					wait(0);
				}
			}

			free(instruccion);
			free(buffer);
			return h;
		} 
	}
}

struct hijos * procesarRaiz(struct hijos * h,int lectura)
{
	char * comando = NULL;
	char * path1 = NULL;
	char * path2 = NULL;
	char * buffer = malloc(520*sizeof(char));
	char * instruccion = malloc(520*sizeof(char));
	char * resultado = malloc(720*sizeof(char));
	strcpy(resultado,"\0");
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
				printf("Uso: ls <path>\n");
				continue;
			}
			if (strcmp(path1,"/") == 0)
			{
				ls(".");
				continue;
			}
			char * obj = NULL;
			char * sig = NULL;
			obj = strtok(path1,"/");
			int i;
			int escrito = 0;
			if (h != NULL)
			{
				for (i = 0; i <  h-> n; ++i)
				{
					if (strcmp(h->nombres[i],obj) == 0)
					{
						memset(instruccion,0,520);
						strcpy(instruccion,"ls /");
						while( (sig=strtok(NULL,"/")) != NULL)
						{
							strcat(instruccion,sig);
							strcat(instruccion,"/");
						}
						write(h->pipes[i],instruccion,520);
						escrito = 1;
					}
				}
			}
			if (escrito)
			{
				memset(resultado,0,720);
				read(lectura,resultado,720);
				printf("%s",resultado);

			} 
			else 
			{
				sig = strtok(NULL,"/");
				if (sig != NULL) printf("Path invalido\n");
				else {
					ls(obj);
				}
			}

		} 
		else if (strcmp(comando,"cat") == 0)
		{
			if (path1 == NULL || path2 != NULL)
			{
				printf("Uso: cat <path>\n");
				continue;
			}
			
			char * obj = NULL;
			char * sig = NULL;
			obj = strtok(path1,"/");
			sig = strtok(NULL,"/");
			if (sig == NULL)
			{
				cat(obj);
				continue;
			}


			int i;
			int escrito = 0;
			if (h != NULL)
			{	
				for (i = 0; i <  h-> n; ++i)
				{
					if (strcmp(h->nombres[i],obj) == 0)
					{
						memset(instruccion,0,520);
						strcpy(instruccion,"cat /");
						strcat(instruccion,sig);
						while( (sig=strtok(NULL,"/")) != NULL)
						{
							strcat(instruccion,"/");
							strcat(instruccion,sig);
						}
						write(h->pipes[i],instruccion,520);
						escrito = 1;
					}
				}
			}
			if (escrito)
			{
				memset(resultado,0,720);
				read(lectura,resultado,720);
				printf("%s",resultado);

			} 
			else 
			{
				printf("Path invalido.\n");				
			}
		} 
		else if (strcmp(comando,"cp") == 0)
		{
			printf("No implementado :(\n");
		} 
		else if (strcmp(comando,"mv") == 0)
		{
			printf("No implementado :(\n");
		} 
		else if (strcmp(comando,"find") == 0)
		{
			printf("No implementado :(\n");
		} 
		else if (strcmp(comando,"rm") == 0)
		{
			if (path1 == NULL || path2 != NULL)
			{
				printf("Uso: rm <path>\n");
				continue;
			}
			
			char * obj = NULL;
			char * sig = NULL;
			obj = strtok(path1,"/");
			sig = strtok(NULL,"/");
			if (sig == NULL)
			{
				rm(obj);
				continue;
			}
			int i;
			int escrito = 0;
			if (h != NULL)
			{
				for (i = 0; i <  h-> n; ++i)
				{
					if (strcmp(h->nombres[i],obj) == 0)
					{
						memset(instruccion,0,520);
						strcpy(instruccion,"rm /");
						strcat(instruccion,sig);
						while( (sig=strtok(NULL,"/")) != NULL)
						{
							strcat(instruccion,"/");
							strcat(instruccion,sig);
						}
						write(h->pipes[i],instruccion,520);
						escrito = 1;
					}
				}
			}
			if (escrito)
			{
				memset(resultado,0,720);
				read(lectura,resultado,720);
				printf("%s",resultado);

			} 
			else 
			{
				printf("Path invalido.\n");				
			}
		} 
		else if (strcmp(comando,"mkdir") == 0)
		{
			if (path1 == NULL || path2 != NULL)
			{
				printf("Uso: mkdir <path>\n");
				continue;
			}
			char * obj = NULL;
			char * sig = NULL;
			obj = strtok(path1,"/");
			sig = strtok(NULL,"/");
			if (sig == NULL)
			{
				if (mkdir(obj,0777) == -1)
				{
					if (errno == 17)
					{
						printf("Ya existe un archivo con nombre %s.\n", obj);
						continue;
					}
					else
					{
						printf("Error en mkdir: %s\n",strerror(errno));
						continue;
					}
				} 
				else 
				{
					int fd[2];
					pipe(fd);
					if (fork() == 0)
					{
						close(fd[1]);
						dup2(fd[0],0);
						dup2(5,1);
						close(fd[0]);
						chdir(obj);
						//liberarHijos(h);
						h = procesarHijo(NULL);
						// Cuando salga de la funcion esta terminando.
						free(instruccion);
						free(buffer);
						return h;
					}
					else
					{
						close(fd[0]);
						if (h != NULL)
						{
							h->nombres = realloc(h->nombres,(h->n+1)*sizeof(char*));
							h->pipes = realloc(h->pipes,(h->n+1)*sizeof(int));
							h->nombres[h->n] = malloc(strlen(obj)*sizeof(char));
							strcpy(h->nombres[h->n],obj);
							h->pipes[h->n] = fd[1]; 
							h->n ++;
						}
						else {
							h = malloc(sizeof(struct hijos));
							h->nombres = malloc(1 * sizeof(char*));
							h->pipes = malloc(1 * sizeof(int));
							h->n=1;
							h->nombres[0] = malloc(strlen(obj)*sizeof(char));
							strcpy(h->nombres[0],obj);
							h->pipes[0] = fd[1];
						}
					}
				}
				continue;
			}
			int i;
			int escrito = 0;
			if (h != NULL)
			{
				for (i = 0; i <  h-> n; ++i)
				{
					if (strcmp(h->nombres[i],obj) == 0)
					{
						memset(instruccion,0,520);
						strcpy(instruccion,"mkdir /");
						strcat(instruccion,sig);
						while( (sig=strtok(NULL,"/")) != NULL)
						{
							strcat(instruccion,"/");
							strcat(instruccion,sig);
						}
						write(h->pipes[i],instruccion,520);
						escrito = 1;
					}
				}
			}
			if (escrito)
			{
				memset(resultado,0,720);
				read(lectura,resultado,720);
				printf("%s",resultado);

			} 
			else 
			{
				printf("Path invalido.\n");				
			}
		} 
		else if (strcmp(comando,"rmdir") == 0)
		{
			printf("No implementado :(\n");
		} 
		else if (strcmp(comando,"quit") == 0)
		{
			if (h != NULL)
			{
				int i;
				for (i = 0; i < h->n; ++i)
				{
					write(h->pipes[i],"quit",10);
				}
				for (i = 0; i < h->n; ++i)
				{
					wait(0);
				}
			}

			free(instruccion);
			free(buffer);
			free(resultado);
			return h;
		} 
		else 
		{
			printf("Comando invalido.\n");
		}
	}
}
