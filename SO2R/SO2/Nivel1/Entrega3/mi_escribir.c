/**
 * Equipo: Alea iacta est
 * Alzamora Llabata, Pere
 * Arenas Afán de Rivera, Francisco
 * Reyes Gómez, Andrea Valentina
 */

#include "directorios.h"

int main(int argc, char **argv){
    int nbytes, longtext;
    char buff_texto[strlen(argv[3])];

    if (argc != 5){
        fprintf(stderr, "Sintaxis: ./mi_escribir <disco> </ruta_fichero> <texto> <offset>\n");
        return -1;
    }
    if (bmount(argv[1]) < 0){
            return -1;
    }
    longtext = strlen(argv[3]);
    if(argv[2][strlen(argv[2])-1]=='/') { // Se verifica que sea un fichero
        fprintf(stderr,"La ruta introducida no se trata de un fichero\n");
        return -1;
    } else {
        strcpy(buff_texto, argv[3]);
        fprintf(stderr, "Longitud del texto: %d\n\n", longtext);

        nbytes = mi_write(argv[2],&buff_texto,atoi(argv[4]),longtext);

        if(nbytes<0){
            fprintf(stderr,"Error de escritura de mi_write()\n");
            return -1;
        } else {
            fprintf(stderr,"Bytes escritos: %d\n",nbytes);
        }

        if (bumount() < 0){ // Desmontar dispositivo
            return -1;
        }
        return 0;
    }
}