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
 * Comprueba funcionalidades de cambio de permisos de un inodo
 * Input        argc: número de parámetros
 *              argv: vector de punteros a los parámetros
 * Output       0: ejecución correcta
 *             -1: error de ejecución
 */
int main(int argc, char **argv){
    if (argc == 4){
        if (bmount(argv[1]) >= 0){ // Montar el dispositivo
            mi_chmod_f(atoi(argv[2]),atoi(argv[3]));
        }else{
            return -1;
        }
        if(bumount(argv[1]) < 0){ // Desmontar el dispositivo
			return -1;
        }
    }else{
        fprintf(stderr,"\nSintaxis: permitir <nombre_dispositivo> <ninodo> <permisos>\n");
        return -1;
    }
    return 0;
}