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
	int tambuffer = BLOCKSIZE*4, offset = 0, acum = 0, leidos = 0, error;
	char buffer_texto[tambuffer];
	struct STAT p_stat;
	unsigned int p_inodo, p_entrada, p_inodo_dir;

	if (argc != 3){
		fprintf(stderr, "\nSintaxis: ./mi_cat <disco> </ruta_fichero>\n");
		return -1;
	} else {
		if (bmount(argv[1]) < 0){ // Montar el dispositivo
            return -1;
        } else { 
            if((argv[2][strlen(argv[2])-1])=='/'){ // Comprobar que es un fichero
                fprintf(stderr,"Error: la ruta no corresponde a un fichero\n");
                return -1;
            }

			memset(buffer_texto, 0, tambuffer);

			leidos = mi_read(argv[2], buffer_texto, offset, tambuffer);
			while (leidos > 0)
			{
				write(1, buffer_texto, leidos);
				acum += leidos;
				offset += tambuffer;
				memset(buffer_texto, 0, tambuffer);
				leidos = mi_read(argv[2], buffer_texto, offset, tambuffer);
			}
			sprintf(string, "\nTotal_leidos %d\n", acum);
			write(2, string, strlen(string));
            
			if((error = buscar_entrada(argv[2], &p_inodo_dir, &p_inodo, &p_entrada, 0, 0)) <0 ){
				mostrar_error_buscar_entrada(error);
				return -1;
			}
            mi_stat_f(p_inodo,&p_stat); 
			
            if (bumount() < 0)
			{ // Desmontar el dispositivo
				return -1;
			}
		}
		return 0;
	}
	
}