#include "ficheros.h"

int main(int argc, char *argv[])
{
    unsigned int offsets[5] = {9000, 209000, 30725000, 409605000, 480000000};
    int ninodo, nbytes, longtext, dif_inodos;
    struct STAT p_stat;

    char buff_texto[strlen(argv[2])];

    if (argc != 4)
    {
        fprintf(stderr, "Sintaxis correcta: escribir <nombre_dispositivo> <texto> <diferentes_inodos>\n Offsets: 9000, 209000, 30725000, 409605000, 480000000\n"
                        "Si diferentes_inodos = 1 se reserva un inodo distinto para cada uno de los offsets\n");

        return FALLO;
    }

    longtext = strlen(argv[2]);
    dif_inodos = atoi(argv[3]);
    if (bmount(argv[1]) < 0)
    {
        return FALLO;
    }

    strcpy(buff_texto, argv[2]);
    fprintf(stderr, "Longitud del texto: %d\n", longtext);
    if (dif_inodos == 0)
    {
        ninodo = reservar_inodo('f', 6);
        if (ninodo < 0)
        {
            return FALLO;
        }
        for (int i = 0; i < 5; i++)
        {
            fprintf(stderr, " Nº inodo reservado: %d\n", ninodo);
            fprintf(stderr, "Offset: %d\n", offsets[i]);
            nbytes = mi_write_f(ninodo, buff_texto, offsets[i], longtext);
            if (nbytes == FALLO)
            {
                return FALLO;
            }
            fprintf(stderr, "Bytes escritos: %d\n", nbytes);
            if (mi_stat_f(ninodo, &p_stat) == FALLO)
            {
                return FALLO;
            }
        }
    }
    else
    { // Multiples inodos
        for (int i = 0; i < 5; i++)
        {
            ninodo = reservar_inodo('f', 6);
            if (ninodo < 0)
            {
                return FALLO;
            }
            fprintf(stderr, "Nº inodo reservado: %d\n", ninodo);
            fprintf(stderr, "Offset: %d\n", offsets[i]);
            if ((nbytes = mi_write_f(ninodo, buff_texto, offsets[i], longtext)) < 0)
            {
                fprintf(stderr, "Error de escritura\n");
                return FALLO;
            }
            fprintf(stderr, "Bytes escritos: %d\n", nbytes);
            if (mi_stat_f(ninodo, &p_stat) < 0)
            {
                fprintf(stderr, "Error de lectura de STAT\n");
                return FALLO;
            }
            fprintf(stderr, "stat.tamEnBytesLog = %d\nstat.numBloquesOcupados = %d\n\n", p_stat.tamEnBytesLog, p_stat.numBloquesOcupados);
        }
    }
    if (bumount() < 0)
    {
        return FALLO;
    }
    return EXITO;
}