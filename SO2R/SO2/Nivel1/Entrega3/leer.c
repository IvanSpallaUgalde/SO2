/**
 * Equipo: Alea iacta est
 * Alzamora Llabata, Pere
 * Arenas Afán de Rivera, Francisco
 * Reyes Gómez, Andrea Valentina
 */

//#include "ficheros.h"
#include "directorios.h"

/*
 * Función: main()
 * Comprueba funcionalidades de lectura
 * Input        argc: número de parámetros
 *              argv: vector de punteros a los parámetros
 * Output       0: ejecución correcta
 *              -1: error de ejecución
 */
int main(int argc, char **argv)
{
	char string[128];
	int tambuffer = 1500, offset = 0, acum = 0, ninodo, leidos = 0;
	char buffer_texto[tambuffer];
	struct STAT p_stat;
	if (argc >= 3)
	{
		if (bmount(argv[1]) >= 0)
		{ // Montar el dispositivo
			ninodo = atoi(argv[2]);
			memset(buffer_texto, 0, tambuffer);

			leidos = mi_read_f(ninodo, buffer_texto, offset, tambuffer);
			while (leidos > 0)
			{
				write(1, buffer_texto, leidos);
				acum += leidos;
				offset += tambuffer;
				memset(buffer_texto, 0, tambuffer);
				leidos = mi_read_f(ninodo, buffer_texto, offset, tambuffer);
			}

			sprintf(string, "\ntotal_leidos %d\n", acum);
			write(2, string, strlen(string));

			mi_stat_f(ninodo, &p_stat);
			sprintf(string, "tamEnBytesLog %d\n", p_stat.tamEnBytesLog);
			write(2, string, strlen(string));

			if (bumount(argv[1]) < 0)
			{ // Desmontar el dispositivo
				return -1;
			}
		}
		else
		{
			return -1;
		}
		return 0;
	}
	else
	{
		fprintf(stderr, "\nSintaxis: leer <nombre_dispositivo> <ninodo>\n");
		return -1;
	}
}