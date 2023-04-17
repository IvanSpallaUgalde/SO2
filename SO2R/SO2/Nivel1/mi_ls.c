/**
 * Equipo: Alea iacta est
 * Alzamora Llabata, Pere
 * Arenas Afán de Rivera, Francisco
 * Reyes Gómez, Andrea Valentina
 */

#include "directorios.h"


int main(int argc, char *argv[]){
    //TIENE MEJORAS OPCIONALES
    char buff[TAMBUFFER];
    int num_entradas;
	if(argc!=3){
		fprintf(stderr,"Sintaxis ./mi_ls <nombre_dispositivo> </ruta_directorio>");
		return -1;
	}
    if(bmount(argv[1])<0){ // Montar disco
		return -1;
	}
    num_entradas = mi_dir(argv[2],buff);
	if(num_entradas < 0){ //No hay entradas
		return -1;
	}
	printf("Total: %d\n",num_entradas);
	printf("%c[%d;%dmTipo\tModo\tmTime\t\t\tTamaño\tNombre%c[%dm\n",27,0,34,27,0);
	printf("-----------------------------------------------------------\n");
    printf("%s\n",buff);
    bumount(); // Desmontar disco
    return 0;
}