#include "directorios.h"

int main(int argc, char const *argv[])
{
    if (argc != 3)
    {
        fprintf(stderr, "Sintaxis correcta: ./mi_ls <disco> </ruta_directorio>\n");
        return FALLO;
    }

    char buf[TAMBUFFER];

    if(bmount(argv[1]) == FALLO)
    {
        return FALLO;
    }

    int numero_entradas =mi_dir(argv[2],buf);

    if (numero_entradas < 0)
    {
        return FALLO;
    }
    
    printf("%s\n",buf);
    printf("Numero entradas: %d",numero_entradas);

    bumount();

}