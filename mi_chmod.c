#include "directorios.h"

int main(int argc, char const *argv[])
{
    if (argc != 4)
    {
        fprintf(stderr, "Sintaxis correcta: ./mi_chmod <disco> <permisos> </ruta>\n");
        return FALLO;
    }

    //Obtenemos los permisos
    int permisos = atoi(argv[2]);
    if(permisos >=0 && permisos <=7)
    {   
        
        //Montamos disco
        if(bmount(argv[1]))
        {
            return FALLO;
        }
        mi_chmod(argv[3],permisos); //entonces esto ya esta creo
        bumount;
        return EXITO;
    }
    else
    {
       return FALLO; 
    }
}