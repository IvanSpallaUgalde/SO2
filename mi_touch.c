#include "directorios.h"

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        fprintf(stderr,"Sintaxis ./mi_mkdir <disco> <permisos> </ruta>\n");
        return FALLO;
    }
    if (atoi(argv[2])<0 ||  atoi(argv[2])>7)
    {
        fprintf(stderr, "ERROR: Permisos invalidos\n");
        return FALLO;
    }
    if(argv[3][atoi(argv[3])-1] == '/')
    {
        mostrar_error_buscar_entrada(ERROR_CAMINO_INCORRECTO);
        return FALLO;
    }
    if (bmount(argv[1] == FALLO))
    {
        return FALLO;
    }
    mi_creat(argv[3], atoi(argv[2]));
    bumount();
    return EXITO;
}