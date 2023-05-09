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

    //buscamos la entrada cuyo nombre se encuentra en inicial
    leer_inodo(p_inodo_dir,&inodo_dir);
    
    if((inodo_dir.permisos & 4) !=4){
        return ERROR_PERMISO_LECTURA;
    }

    //Iniciar el buffer de lectura !!!!

    //Calculamos la contidad de entradas en el inodo (no estoy seguro)
    cant_entradas_inodo=(inodo_dir.numBloquesOcupados*BLOCKSIZE)/sizeof(entrada);
    num_entrada_inodo=0;
    if (cant_entradas_inodo>0)
    {
        //Leer entrada

        while((num_entrada_inodo < cant_entradas_inodo)&&(inicial != entrada.nombre))
        {
            num_entrada_inodo++;
            //Leer siguiente entrada
        }
    }

    if((inicial != entrada.nombre) && (num_entrada_inodo = cant_entradas_inodo))
    {
        //La entrada no exsite
        switch(reservar)
        {
            case 0: //Modo consulta, al no existir retornamos error
                return ERROR_NO_EXISTE_ENTRADA_CONSULTA;
                break;
            case 1: //Modo escritura
            //Creamos la entrada en el directorio referenciado por *p_inodo_dir
            //si es fichero no permitir escritura
            if(inodo_dir.tipo == 'f')
            {
                return ERROR_NO_SE_PUEDE_CREAR_ENTRADA_EN_UN_FICHERO;
            }

            if((inodo_dir.permisos & 4) != 4)
            {
                return ERROR_PERMISO_ESCRITURA;
            }
            else
            {
                //Copiar *inicial en el nombre de la entrada!!

                if(inodo_dir.tipo == 'd')
                {
                    if(final == "/")
                    {
                        //Reservar un inodo como directorio y asignarlo a la entrada
                        reservar_inodo('d',/*Permisos, por ahora permiross para todo*/7);
                        
                    }
                    else
                    {
                        return ERROR_NO_EXISTE_DIRECTORIO_INTERMEDIO;
                    }
                }
                else //Se trata de un fichero
                {
                    //Reservar un inodo como fichero y asignarlo a la entrada
                    reservar_inodo('f',/*Permisos, por ahora permiross para todo*/7);
                }
                //Escribir la entrada en el directorio padre
                if(/*Error escritura*/)
                {
                    if(/*Se habia reservado un inodo para la entrada*/)
                    {
                        //Liberar inodo
                    }
                    return FALLO;
                }
            }
        }
            
    }   
    //Si hemos llegado al final del camino (No estoy seguro)
    if(camino_parcial==NULL)
    {
        if((num_entrada_inodo < cant_entradas_inodo)&&(reservar == 1))
        {
            return ERROR_ENTRADA_YA_EXISTENTE;
        }
        //Asignar a *p_inodo el numero de inodo del directorio o fichero creado o leido
        //Asignar a *p_netrada el numero de su entrada dentro del ultimo diretorio que lo contiene
        return EXITO;
    }
    else
    {
        //Asingar a *p_inodo_dir el puntero al inodo que se indica en la entrada encontrada
        return buscar_entrada(final,p_inodo_dir,p_inodo,p_entrada,reservar,permisos);
    }
    return EXITO;
    
    

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
