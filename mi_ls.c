#include "directorios.h"

#define TAMFILA 100
#define TAMBUFFER (TAMFILA*1000) //suponemos un máx de 1000 entradas, aunque debería ser SB.totInodos

int main(int argc, char const *argv[])
{
    if (argc != 3)
    {
        fprintf(stderr, "Sintaxis correcta: ./mi_ls <disco> </ruta_directorio>\n");
        return FALLO;
    }

    unsigned char buf[TAMBUFFER];

    bmount(argv[1]);
    
    int numero_entradas =mi_dir(argv[2],buf);
    printf("%s\n",buf);
    printf("Numero entradas: %d",numero_entradas);

    bumount;

}