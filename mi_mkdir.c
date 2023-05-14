#include "directorios.h"


int main(int argc, char const *argv[])
{
    
    if (argc != 4)
    {
        fprintf(stderr, "Sintaxis correcta: ./mi_mkdir <disco> <permisos> </ruta>\n");
        return FALLO;
    }
    
    if (atoi(argv[2]) < 0 || atoi(argv[2]) > 7)
    {
        fprintf(stderr, "Error: los permisos indicados no son validos\n");
        return FALLO;
    }
    
    unsigned char permisos = atoi(argv[2]);

    if (argv[3][atoi(argv[3]) - 1] == '/')
    {
        mostrar_error_buscar_entrada(ERROR_CAMINO_INCORRECTO);
        return FALLO;
    }
    bmount(argv[1]);
    mi_creat(argv[3], atoi(argv[2]));
    bumount;
    return EXITO;
}