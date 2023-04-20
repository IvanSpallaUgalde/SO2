#include <stdlib.h>
#include "ficheros.h"


int main(int argc, char const *argv[])
{
    //Declaracion de variables
    char string[128];
    int tambuffer = 1500, offset = 0, count = 0, ninodo, nbytes_leidos = 0;
    char buffer_texto[tambuffer];
    struct STAT p_stat;
    if (argc >= 3)
    {
        if (bmount(argv[1] >= 0))
        {
            ninodo = atoi(argv[2]);
            memset(buffer_texto, 0, tambuffer);

            nbytes_leidos = mi_read_f(ninodo, buffer_texto, offset, tambuffer);
            while (nbytes_leidos > 0)
            {
                write(1, buffer_texto, nbytes_leidos);
                count += nbytes_leidos;
                offset += tambuffer;
                memset(buffer_texto, 0, tambuffer);
                nbytes_leidos = mi_read_f(ninodo, buffer_texto, offset, tambuffer);
            }

            sprintf(string, "\nbytes_leidos %d\n", count);
            write(2, string, strlen(string));

            mi_stat_f(ninodo, &p_stat);
            sprintf(string, "tamEnBytesLog %d\n", p_stat.tamEnBytesLog);
            write(2, string, strlen(string));

            if (bumount(argv[1]) < 0)
            {
                fprintf(stderr, "Erro en bumount\n");
                return FALLO;
            }
            
        }
        else
        {
            fprintf(stderr,"Error en bmount\n");
            return FALLO;
        }
        return EXITO;
        
    }
    else
    {
        fprintf(stderr, "Sintaxis correcta: leer <nombre_dispositivo> <ninodo>\n");
        return FALLO;
    }
    

}