
#include "ficheros_basico.h"

unsigned char buffer_memoria[BLOCKSIZE];

int main(int argc,char **argv){

    if (argc != 3)
    {
        fprintf(stderr, "Error de sintaxis. Sintaxis correcta: ./mi_mkfs <nombre del fichero> <numero de bloques>\n");
        return FALLO;
    }

    //Parametros de argv
    char *path = argv[1];
    int nbloques = atoi(argv[2]);
    int ninodos = nbloques/4;

    //Init buff
    unsigned char buf[BLOCKSIZE];
    if (!memset(buf, 0, BLOCKSIZE))
    {
        return FALLO;
    }

    //Montaje
    if (bmount(path) == FALLO)
    {
        fprintf(stderr, "Error al montar el dispositivo \n");
        return FALLO;
    }
    
    //Escritura
    for (int i = 0; i < nbloques; i++)
    {
        if (bwrite(i, buf) == FALLO)
        {
            fprintf(stderr, "ERror al escribir en el dispositivo en la posicion %i\n", i);
            return FALLO;
        }
        
    }
    
    //Init metadatos
    if (initSB(nbloques, ninodos) == FALLO)
    {
        fprintf(stderr, "Error inicializando el superbloque");
        return FALLO;
    }
    
    if (initMB() == FALLO)
    {
        fprintf(stderr, "Error inicializando el mapa de bits");
        return FALLO;
    }

    if(initAI() == FALLO)
    {
        fprintf(stderr, "Error inicializando la array de inodos");
        return FALLO;
    }
    
    //Cerramos el fichero
    if (bumount() == FALLO)
    {
        fprintf(stderr, "Error al desmontar el dispositivo \n");
        return FALLO;
    }
    return EXITO;
}