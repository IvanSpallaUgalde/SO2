/**
 * Equipo: Alea iacta est
 * Alzamora Llabata, Pere
 * Arenas Afán de Rivera, Francisco
 * Reyes Gómez, Andrea Valentina
 */

#include "directorios.h"

int main(int argc, char *argv[]){
	if(argc!=3){
        fprintf(stderr,"Sintaxis: ./mi_rm <disco> </ruta>\n");
        return -1;
    }
    if(argv[2][strlen(argv[2])-1]!='/'){
        fprintf(stderr,"Error: la ruta no corresponde a un directorio\n");
    }
    if(bmount(argv[1])<0) return -1;
    if(mi_unlink(argv[2])<0) return -1;
    if(bumount()<0) return -1;
    return 0;
}