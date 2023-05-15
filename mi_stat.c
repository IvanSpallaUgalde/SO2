#include "directorios.h"

int main(int argc, char const *argv[])
{
    struct STAT stat;
    if(argc != 3)
    {
        fprintf(stderr, "Sintaxis ./mi_stat <disco> </ruta>\n");
        return FALLO;
    }

    if ((bmount(argv[1])) == FALLO)
    {
        return FALLO;
    }
    mi_stat(argv[2], &stat);
    bumount();
    return EXITO;
}