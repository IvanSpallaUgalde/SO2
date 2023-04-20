#include "ficheros.h"

int main(int argc, char *argv[])
{
    int ninodo = atoi(argv[2]);
    int permisos = atoi(argv[3]);

    if(argc != 4)
    {
        fprintf(stderr, "La sintaxis correcta es: permitir <nombre_dispositivo> <ninodo> <permisos>\n");
        return FALLO;
    }

    if (bmount(argv[1]) == FALLO)
    {
        fprintf(stderr, "Error montando el sistema en permitir.c\n");
        return FALLO;
    }

    if (mi_chmod_f(ninodo, permisos) == FALLO)
    {
        fprintf(stderr, "Error de mi_chmod_f en permitir.c\n");
        return FALLO;
    }

    if(bumount() == FALLO)
    {
        fprintf(stderr, "Error desmontando el sistema en permitir.c\n");
        return FALLO;
    }
    
    return EXITO;
}