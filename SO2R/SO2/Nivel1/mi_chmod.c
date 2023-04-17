/**
 * Equipo: Alea iacta est
 * Alzamora Llabata, Pere
 * Arenas Afán de Rivera, Francisco
 * Reyes Gómez, Andrea Valentina
 */

#include "directorios.h"

int main(int argc, char *argv[]){
	if(argc!=4){
		fprintf(stderr,"Sintaxis ./mi_chmod <nombre_dispositivo> <permisos> </ruta>\n");
		return -1;
	}
    if(atoi(argv[2])<0 && atoi(argv[2])>7){
		fprintf(stderr,"ERROR: Permisos inválidos\n");
		return -1;
    }
    bmount(argv[1]);
    mi_chmod(argv[3],atoi(argv[2]));
    bumount();
    return 0;
}