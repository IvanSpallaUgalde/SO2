/**
 * Equipo: Alea iacta est
 * Alzamora Llabata, Pere
 * Arenas Afán de Rivera, Francisco
 * Reyes Gómez, Andrea Valentina
 */

#include "directorios.h"

int main(int argc, char **argv)
{
    unsigned int offsets[5] = {9000, 209000, 30725000, 409605000, 480000000};
    int ninodo, nbytes, longtext, dif_inodos;
    struct STAT stat;

    char buff_texto[strlen(argv[2])];

    if (argc != 4)
    {
        fprintf(stderr, "Sintaxis: escribir <nombre_dispositivo> "
                        "<texto>"
                        " <diferentes_inodos>\nOffsets: 9000, 209000, 30725000, 409605000, 480000000\nSi diferentes_inodos=1 se reserva un inodo distinto para cada uno de los offsets\n");
        return -1;
    }

    longtext = strlen(argv[2]);
    dif_inodos = atoi(argv[3]);
    if (bmount(argv[1]) < 0)
    {
        return -1;
    }
    strcpy(buff_texto, argv[2]);
    fprintf(stderr, "Longitud del texto: %d\n\n", longtext);
    if (dif_inodos == 0)
    { //Solo un inodo
        ninodo = reservar_inodo('f', 6);
        if (ninodo < 0)
        {
            return -1;
        }
        for (int i = 0; i < 5; i++)
        {
            fprintf(stderr, "Nº inodo reservado: %d\n", ninodo);
            fprintf(stderr, "Offset: %d\n", offsets[i]);
            if ((nbytes = mi_write_f(ninodo, buff_texto, offsets[i], longtext)) < 0)
            {
                fprintf(stderr, "Error de escritura\n");
                return -1;
            }
            fprintf(stderr, "Bytes escritos: %d\n", nbytes);
            if (mi_stat_f(ninodo, &stat) < 0)
            {
                fprintf(stderr, "Error de lectura de STAT\n");
                return -1;
            }
            fprintf(stderr, "stat.tamEnBytesLog = %d\nstat.numBloquesOcupados = %d\n\n", stat.tamEnBytesLog, stat.numBloquesOcupados);
        }
    }
    else
    { //Multiples inodos
        for (int i = 0; i < 5; i++)
        {
            ninodo = reservar_inodo('f', 6);
            if (ninodo < 0)
            {
                return -1;
            }
            fprintf(stderr, "Nº inodo reservado: %d\n", ninodo);
            fprintf(stderr, "Offset: %d\n", offsets[i]);
            if ((nbytes = mi_write_f(ninodo, buff_texto, offsets[i], longtext)) < 0)
            {
                fprintf(stderr, "Error de escritura\n");
                return -1;
            }
            fprintf(stderr, "Bytes escritos: %d\n", nbytes);
            if (mi_stat_f(ninodo, &stat) < 0)
            {
                fprintf(stderr, "Error de lectura de STAT\n");
                return -1;
            }
            fprintf(stderr, "stat.tamEnBytesLog = %d\nstat.numBloquesOcupados = %d\n\n", stat.tamEnBytesLog, stat.numBloquesOcupados);
        }
    }
    if (bumount() < 0)
    {
        return -1;
    }
    return 0;
}