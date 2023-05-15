#include "directorios.h"

int main(int argc, char **argv)
{
    char string[128];
    int tambuffer = BLOCKSIZE * 4, offset = 0, count = 0, leidos = 0, error;
    char buffer[tambuffer];
    struct STAT p_stat;
    unsigned int p_inodo, p_entrada, p_inodo_dir;

    if (argc != 3)
    {
        fprintf(stderr, "Sintaxis: ./mi_cat <disco> </ruta_fichero>\n");
        return FALLO;
    }
    if (bmount(argv[1]) == FALLO)
    {
        return FALLO;
    }
    if(argv[2][strlen(argv[2])-1] == '/')
    {
        fprintf(stderr,"Error: la ruta indicada no es un fichero\n");
        return FALLO;
    }
    memset(buffer, 0, tambuffer);

    leidos = mi_read(argv[2], buffer, offset, tambuffer);
    while (leidos > 0)
    {
        write(1,buffer, leidos);
        count += leidos;
        offset += tambuffer;
        memset(buffer, 0, tambuffer);
        leidos = mi_read(argv[2],buffer,offset,tambuffer);
    }
    sprintf(string, "Total_leidos %d\n", count);
    write(2, string, strlen(string));

    if((error = buscar_entrada(argv[2], &p_inodo_dir, &p_inodo, &p_entrada, 0, 0)) < 0)
    {
        mostrar_error_buscar_entrada(error);
        return FALLO;
    }
    mi_stat_f(p_inodo,&p_stat);
    bumount();
    return EXITO;
}