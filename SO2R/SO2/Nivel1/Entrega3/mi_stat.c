/**
 * Equipo: Alea iacta est
 * Alzamora Llabata, Pere
 * Arenas Afán de Rivera, Francisco
 * Reyes Gómez, Andrea Valentina
 */

#include "directorios.h"

int main(int argc, char *argv[]){
    struct STAT stat;
	if(argc!=3){
		fprintf(stderr,"Sintaxis ./mi_stat <nombre_dispositivo> </ruta>\n");
		return -1;
	}
    bmount(argv[1]);
    mi_stat(argv[2],&stat);
    bumount();
    return 0;
}