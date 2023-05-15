#include "directorios.h"

int main(int argc, char **argv)
{
    int nbytes, longtext;
    char buff[strlen(argv[3])];

    if (argc != 5)
    {
        fprintf(stderr, "Sintaxis: mi_escribir <nombre_dispositivo> </ruta_fichero> <texto> <offset> \n");
        return FALLO;
    }
    if(bmount(argv[1]) == FALLO)
    {
        return FALLO;
    }
    longtext = strlen(argv[3]);
    if (argv[2][strlen(argv[2])-1] == '/')
    {
        fprintf(stderr, "La ruta introducida no es un fichero");
        return FALLO;
    }
    else
    {
        strcpy(buff, argv[3]);
        fprintf(stderr, "Longitud del texto: %d\n\n");

        nbytes = mi_write(argv[2], &buff, atoi(argv[4]), longtext);

        if (nbytes < 0)
        {
            fprintf(stderr, "Error de escritura de mi_write() [mi_escribir.c]\n");
            return FALLO;
        }
        else
        {
            fprintf(stderr, "Bytes escritos: %d\n", nbytes);
        }

        if (bumount() < 0)
        {
            return FALLO;
        }
        return EXITO;
    }

}