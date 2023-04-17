/**
 * Equipo: Alea iacta est
 * Alzamora Llabata, Pere
 * Arenas Afán de Rivera, Francisco
 * Reyes Gómez, Andrea Valentina
 */

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
	
    if(argc!=4){
        fprintf(stderr,"Sintaxis: ./mi_link <disco> </ruta_fichero_original> </ruta_enlace>\n");
        return -1;
    }
    
    if(argv[2][strlen(argv[2])-1]=='/'){
        fprintf(stderr,"/ruta_fichero_original no es una ruta a un fichero\n");
        return -1;
    }
    
    if(argv[3][strlen(argv[3])-1]=='/'){
        fprintf(stderr,"/ruta_enlace no es una ruta a un fichero\n");
        return -1;
    }
    if(bmount(argv[1])<0) return -1;
    if(mi_link(argv[2],argv[3])<0) return -1;
    if(bumount()<0) return -1;
    return 0;
    
}