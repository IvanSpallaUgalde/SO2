#include "directorios.h"
#include "string.h"

//Esta mal 99%
int extraer_camino(const char *camino, char *inicial, char *final, char *tipo)
{
    char *token;
    char *aux;
    const char barra ="/";

    strcpy(aux,camino);
    //Eliminamos el primer "/"
    token=strtok(aux,barra);

    token = strtok(NULL, barra);

    if(strcpy(inicial,token)!=NULL)
    {
        strcpy(tipo,"d");
    }
    else
    {
        strcpy(tipo,"f");
    }

    if(strtok(token,barra)!=NULL)
    {
        strcpy(final,token);
    }
    else
    {
        strcpy(final,"");
    }

    
}



int buscar_entrada(const char *camino_parcial, unsigned int *p_inodo_dir, unsigned int *p_inodo, unsigned int *p_entrada, char reservar, unsigned char permisos)
{
    struct entrada entrada;
    struct inodo inodo_dir;
    char inicial[sizeof(entrada.nombre)];
    char final[strlen(camino_parcial)];
    char tipo;
    int cant_entradas_inodo, num_entrada_inodo;

    //Si es el directorio raiz
    if (strcmp(camino_parcial,"/")==0)
    {      
        struct superbloque SB;
        if (bread(posSB,&SB)==0)
        {
            return FALLO;
        }
        
        p_inodo=SB.posInodoRaiz;
        p_entrada=0;
        return 0;
    }

    if(extraer_camino(camino_parcial,inicial,final,&tipo)==FALLO)
    {
        return ERROR_CAMINO_INCORRECTO;
    }

    //buscamos la enmtrada cuyo nombre se encuentra en inicial
    leer_inodo(p_inodo_dir,&inodo_dir);
    if(inodo_dir.permisos/*MIRAR COMO SE COMPRUEBAN LOS PERMISOS!!*/){
        return ERROR_PERMISO_LECTURA;
    }

    //Iniciar el buffer de lectura !!!!

    //Calculamos la contidad de entradas en el inodo (no estoy seguro)
    cant_entradas_inodo=(inodo_dir.numBloquesOcupados*BLOCKSIZE)/sizeof(entrada);
    num_entrada_inodo=0;
    if (cant_entradas_inodo>0)
    {
        
    }
    
    

}

//Función auxiliar para printear los posibles errores
void mostrar_error_buscar_entrada(int error) {
   // fprintf(stderr, "Error: %d\n", error);
   switch (error) {
   case -2: fprintf(stderr, "Error: Camino incorrecto.\n"); break;
   case -3: fprintf(stderr, "Error: Permiso denegado de lectura.\n"); break;
   case -4: fprintf(stderr, "Error: No existe el archivo o el directorio.\n"); break;
   case -5: fprintf(stderr, "Error: No existe algún directorio intermedio.\n"); break;
   case -6: fprintf(stderr, "Error: Permiso denegado de escritura.\n"); break;
   case -7: fprintf(stderr, "Error: El archivo ya existe.\n"); break;
   case -8: fprintf(stderr, "Error: No es un directorio.\n"); break;
   }
}
