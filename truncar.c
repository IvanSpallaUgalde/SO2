#include "ficheros.h"

int main(int argc, char *argv[])
{
    int ninodo, nbytes;
    if (argc != 4)
    {
        fprintf(stderr, "Sintaxis correcta: truncar <nombre_dispositivo> <ninodo> <nbytes>\n");
        return FALLO;
    }

    ninodo = atoi(argv[2]);
    nbytes = atoi(argv[3]);

    if (bmount(argv[1]) == FALLO)
    {
        return FALLO;
    }
    
    if(nbytes == 0)
    {
        if(liberar_inodo(ninodo) == FALLO)
        {
            fprintf(stderr, "Error libarando el inodo en truncar.c\n");
            return FALLO;
        }
    }
    else
    {
        mi_truncar_f(ninodo, nbytes);
    }
    
    struct STAT p_stat;
    if(mi_stat_f(ninodo, &p_stat) == FALLO)
    {
        fprintf(stderr, "Error en mi_stat_f de truncar.c\n");
        return FALLO;
    }

    printf("tamEnBytesLog: %d\n", p_stat.tamEnBytesLog);
    printf("numBloquesOcupados: %d\n", p_stat.numBloquesOcupados);

    if (bumount() == FALLO)
    {
        fprintf(stderr, "Error desmontando el dispositivo en truncar.c\n");
        return FALLO;
    }

    return EXITO;    
}