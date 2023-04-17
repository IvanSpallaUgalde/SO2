/**
 * Equipo: Alea iacta est
 * Alzamora Llabata, Pere
 * Arenas Afán de Rivera, Francisco
 * Reyes Gómez, Andrea Valentina
 */

#include "directorios.h"

int main(int argc, char *argv[]){
	if(argc!=4){ // Error de sintaxis
		fprintf(stderr,"Sintaxis ./mi_mkdir <nombre_dispositivo> <permisos> </ruta>\n");
		return -1;
	}
    if(atoi(argv[2])<0 || atoi(argv[2])>7){ // Comprobacion de permisos
		fprintf(stderr,"ERROR: Permisos inválidos\n");
		return -1;
    }
	if(argv[3][atoi(argv[3])-1]=='/'){
		mostrar_error_buscar_entrada(ERROR_CAMINO_INCORRECTO);
		return -1;
	}
	bmount(argv[1]);
	mi_creat(argv[3],atoi(argv[2]));
	bumount();
    return 0;
}