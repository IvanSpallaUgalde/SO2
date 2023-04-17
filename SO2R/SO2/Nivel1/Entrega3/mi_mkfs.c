/**
 * Equipo: Alea iacta est
 * Alzamora Llabata, Pere
 * Arenas Afán de Rivera, Francisco
 * Reyes Gómez, Andrea Valentina
 */

#include "bloques.h"
#include "directorios.h"

int fd = 0; // File Descriptor
int bw = 0; // Bytes Written
int nbloques = 0;

/*
 * Función: main()
 * Formatea el dispositivo virtual con el tamaño adecuado de bloques
 * Input        argc: número de parámetros
 *              argv: vector de punteros a los parámetros
 * Output       0: ejecución correcta
 *              -1: error de ejecución
 */
int main(int argc, char **argv){
	char *buf = (char *) malloc (BLOCKSIZE);
    if (bmount(argv[1]) >= 0){
        int i;
        nbloques = atoi(argv[2]);
        for(i=0; i < nbloques; i++){
            memset(buf,0,BLOCKSIZE);
            bw = bwrite(i,buf);
        }
        initSB(nbloques,nbloques/4);
        initMB();
        initAI();
        reservar_inodo('d',7);
        
        if (bumount() == 0){
            return 0;
        }else{
            return -1;
        }
    }else{
        return -1;
    }
}