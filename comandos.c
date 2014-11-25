#include "comandos.h"

void ls (char* archivo)
{
	DIR *dir;
	struct dirent *dirEntry;
	struct stat statbuf;
	char type = '-';
	char *buffer = malloc(sizeof(char)*200);	// Recordar liberar.
	// Verificacion de error.
	if ((dir = opendir(".")) == NULL)	// Retorna.
	{
		sprintf(buffer, "Error aplicando opendir sobre el directorio.");
		write(1, buffer, 50);
		free(buffer);
		return;
	}
	// Verificamos si se aplica a sí mismo o a un archivo en el.
	if (strcmp(archivo,".") == 0)
	{
		char *aux1	= malloc(sizeof(char)*200);		// Recordar liberar.
		char *aux2	= malloc(sizeof(char)*200);		// Recordar liberar.
		// Iterar por el directorio para ir imprimiendo y así pues.
		while ((dirEntry = readdir(dir)) != NULL)
		{
			if (strcmp(dirEntry->d_name, ".") != 0 && strcmp(dirEntry->d_name,"..") != 0)
			{
				// Si no se puede obtener informacion.
				if (stat(dirEntry->d_name, &statbuf) == -1) // Retorna.
				{
					sprintf(buffer, "Error aplicando stat sobre %s.\n", 
														dirEntry->d_name);
					write(1, buffer, strlen(buffer));
					free(buffer);
					return;
				}
				aux1 = buffer;
				// Si si se pudo, se verifica que tipo de archivo es:
				if ((statbuf.st_mode & S_IFMT) == S_IFREG) type = '-';
				else if ((statbuf.st_mode & S_IFMT) == S_IFDIR) type = 'd';
				else if ((statbuf.st_mode & S_IFMT) == S_IFBLK) type = 'b';
				else if ((statbuf.st_mode & S_IFMT) == S_IFCHR) type = 'c';
				else if ((statbuf.st_mode & S_IFMT) == S_IFLNK) type = 'l';
				else if ((statbuf.st_mode & S_IFMT) == S_IFSOCK) type = 's';
				else if ((statbuf.st_mode & S_IFMT) == S_IFIFO) type = 'p';
				// Se crea la entrada:
				struct tm *tm;
				char fechaM[100];
				tm = localtime(&statbuf.st_mtime);
				sprintf(fechaM,"%d %d %d:%d", tm->tm_mon + 1, tm->tm_mday, tm->tm_hour, 
																		tm->tm_min);
				sprintf(aux2,"%c%c%c%c%c%c%c%c%c%c %d %s %s\t%d\t%s\t%s\n",
						type,
						statbuf.st_mode & 256 ? 'r' : '-',
						statbuf.st_mode & 128 ? 'w' : '-',
						statbuf.st_mode & 64  ? 'x' : '-',
						statbuf.st_mode & 32  ? 'r' : '-',
						statbuf.st_mode & 16  ? 'w' : '-',
						statbuf.st_mode & 8   ? 'x' : '-',
						statbuf.st_mode & 4   ? 'r' : '-',
						statbuf.st_mode & 2   ? 'w' : '-',
						statbuf.st_mode & 1   ? 'x' : '-',
						statbuf.st_nlink,
						getpwuid(statbuf.st_uid)->pw_name,
						getgrgid(statbuf.st_gid)->gr_name,
						statbuf.st_size,
						fechaM,
						dirEntry->d_name);
				// Se crea un nuevo espacio para el buffer con el tamanio que ya tenia
				// mas el de la nueva entrada y se #rellena:
				buffer = malloc(strlen(aux1) + strlen(aux2) +2);
				strcpy(buffer, aux1);
				strcat(buffer, aux2);
				aux1 = malloc(strlen(buffer));
			}
		}
		free(aux1);
		free(aux2);
		closedir(dir);
	} else {
		// Iteramos hasta encontrar el archivo o salir.
		while (((dirEntry = readdir(dir)) != NULL) && (strlen(buffer) == 0))
		{
			// Si hay error:
			if (stat(dirEntry->d_name, &statbuf) == -1) // Retorna.
			{
				sprintf(buffer, "Error aplicando stat sobre %s.\n", dirEntry->d_name);
				write(1, buffer, strlen(buffer));
				free(buffer);
				return;
			}
			// Si es la entrada deseada:
			if (strcmp(dirEntry->d_name, archivo) == 0) 
			{
				// Conversion a fecha que puede ser leida por nosotros los #humanos
				struct tm *tm;
				char fechaM[100];
				tm = localtime(&statbuf.st_mtime);
				sprintf(fechaM,"%d %d %d:%d", tm->tm_mon + 1, tm->tm_mday, tm->tm_hour, 
																		tm->tm_min);
				// Se verifica que tipo de archivo es:
				if ((statbuf.st_mode & S_IFMT) == S_IFREG) type = '-';
				else if ((statbuf.st_mode & S_IFMT) == S_IFDIR) type = 'd';
				else if ((statbuf.st_mode & S_IFMT) == S_IFBLK) type = 'b';
				else if ((statbuf.st_mode & S_IFMT) == S_IFCHR) type = 'c';
				else if ((statbuf.st_mode & S_IFMT) == S_IFLNK) type = 'l';
				else if ((statbuf.st_mode & S_IFMT) == S_IFSOCK) type = 's';
				else if ((statbuf.st_mode & S_IFMT) == S_IFIFO) type = 'p';
				// Se crea la entrada:
				sprintf(buffer,"%c%c%c%c%c%c%c%c%c%c %d %s %s\t%d\t%s\t%s\n",
						type,
						statbuf.st_mode & 256 ? 'r' : '-',
						statbuf.st_mode & 128 ? 'w' : '-',
						statbuf.st_mode & 64  ? 'x' : '-',
						statbuf.st_mode & 32  ? 'r' : '-',
						statbuf.st_mode & 16  ? 'w' : '-',
						statbuf.st_mode & 8   ? 'x' : '-',
						statbuf.st_mode & 4   ? 'r' : '-',
						statbuf.st_mode & 2   ? 'w' : '-',
						statbuf.st_mode & 1   ? 'x' : '-',
						statbuf.st_nlink,
						getpwuid(statbuf.st_uid)->pw_name,
						getgrgid(statbuf.st_gid)->gr_name,
						statbuf.st_size,
						fechaM,
						dirEntry->d_name);
			}
		}
		closedir(dir);
		// Si no lo consiguio, hay error:
		if (strlen(buffer) == 0)
		{
			sprintf(buffer, "Error %s: %s no existe en el directorio.\n", archivo, archivo);
			write(1, buffer, strlen(buffer));
			free(buffer);
			return;
		}
	}
	// Y borralo que fue con lapizzzzzz.
	write(1, buffer, strlen(buffer));
	free(buffer);
}

void cat(char *archivo)
{
	DIR *dir;
	struct dirent *dirEntry;
	struct stat statbuf;
	char *buffer;
	opendir(".");
	if ((dir = opendir(".")) == NULL)
	{
		sprintf(buffer, "Error aplicando opendir sobre el directorio.");
		write(1, buffer, 50);
		free(buffer);
		return;
	}
	// Buscamos el archivo.
	while((dirEntry = readdir(dir)) != NULL)
	{
		if (strcmp(dirEntry->d_name, ".") != 0 && strcmp(dirEntry->d_name,"..") != 0)
		{
			
		}
	}
}
int main(int argc, char const *argv[])
{
	char *archivo = ".";
	ls(archivo);
	return 0;
}