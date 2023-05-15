#include "directorios.h"

int main(int argc, char const *argv[])
{
    if (argc != 4)
    {
        fprintf(stderr, "Sintaxis correcta: ./mi_chmod <disco> <permisos> </ruta>\n");
        return FALLO;
    }

    
    //Montamos disco
    bmount(argv[1]);
    //Obtenemos los permisos
    int permisos = atoi(argv[2]);
    if(permisos >=0 && permisos <=7)
    {
        char ruta[strlen(argv[3])];
        //Obtenemos la ruta
        strcpy(ruta,argv[3]);
        //Cambiamos los permisos
        mi_chmod(ruta,permisos);
        bumount;
        return EXITO;
    }
    else
    {
       return FALLO; 
    }
}