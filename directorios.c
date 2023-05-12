#include "directorios.h"
#include "string.h"

//Esta mal 99%
int extraer_camino(const char *camino, char *inicial, char *final, char *tipo)
{
    char *token;
    char *aux;
    const char barra ='/';

    if (camino[0] != barra)
    {
        fprintf(stderr, "Error: El camino debe comenzar por '/'\n");
        return FALLO;
    }

    token = strchr((camino + 1), barra);
    strcpy(tipo,"f");

    
    //Eliminamos el primer "/"
    if (token)
    {
        strncpy(inicial, (camino + 1), (strlen(camino) - streln(token)));
        
        strcpy(final, token);

        if (final[0] == barra)
        {
            strcpy(tipo, "d");
        }
    }
    else
    {
        strcpy(inicial, (camino + 1));

        strcpy(final, "");
    }

    return EXITO;
}


int buscar_entrada(const char *camino_parcial, unsigned int *p_inodo_dir, unsigned int *p_inodo, unsigned int *p_entrada, char reservar, unsigned char permisos)
{
    struct entrada entrada;
    struct inodo inodo_dir;
    char inicial[sizeof(entrada.nombre)];
    char final[strlen(camino_parcial)];
    char tipo;
    int cant_entradas_inodo, num_entrada_inodo;

    memset(inicial, 0, sizeof(entrada.nombre));
    memset(final, 0, strlen(camino_parcial));
    memset(entrada.nombre, 0, sizeof(entrada.nombre));

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

#if DEBUG7
    printf("[buscar_entrada()->inicial: %s, final: %s, reservar: %d]\n", inicial, final, reservar);
#endif
    //buscamos la entrada cuyo nombre se encuentra en inicial
    if(leer_inodo(p_inodo_dir,&inodo_dir) == FALLO)
    {
        return ERROR_PERMISO_LECTURA;
    }

    if((inodo_dir.permisos & 4) !=4){
        return ERROR_PERMISO_LECTURA;
    }

    //Iniciar el buffer de lectura 
    struct entrada buffer_lectura[BLOCKSIZE / sizeof(struct entrada)];
    memset(buffer_lectura, 0, (BLOCKSIZE / sizeof(struct entrada)) * sizeof(struct entrada));

    //Calculamos la contidad de entradas en el inodo (no estoy seguro)
    cant_entradas_inodo = inodo_dir.tamEnBytesLog/sizeof(entrada);
    num_entrada_inodo = 0;

    if (cant_entradas_inodo>0)
    {
        if ((inodo_dir.permisos & 4) != 4)
        {
            return ERROR_PERMISO_LECTURA;
        }
        
        if (mi_read_f(*p_inodo_dir, &entrada, num_entrada_inodo * sizeof(struct entrada), sizeof(struct entrada)) < 0)
        {
            return ERROR_PERMISO_LECTURA;
        }
        
        //Leer entrada

        while((num_entrada_inodo < cant_entradas_inodo)&&(inicial != entrada.nombre))
        {
            num_entrada_inodo++;
            //Leer siguiente entrada
            if (mi_read_f(p_inodo_dir, &entrada, num_entrada_inodo * sizeof(struct entrada), sizeof(struct entrada)) < 0)
            {
                return ERROR_PERMISO_LECTURA;
            }
            
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
                strcpy(entrada.nombre, inicial);

                if(inodo_dir.tipo == 'd')
                {
                    if(strcmp(final, "/"))
                    {
                        //Reservar un inodo como directorio y asignarlo a la entrada
                        entrada.ninodo = reservar_inodo(tipo, permisos);
                        
#if DEBUG7
                        printf("[buscar_entrada()->reservar inodo: %d tipo %c con permisos %d para '%s']\n", entrada.ninodo, tipo, permisos, entrada.nombre)
#endif
                    }
                    else
                    {
                        return ERROR_NO_EXISTE_DIRECTORIO_INTERMEDIO;
                    }
                }
                else //Se trata de un fichero
                {
                    //Reservar un inodo como fichero y asignarlo a la entrada
                    entrada.ninodo = reservar_inodo(tipo, permisos);
                }
                //Escribir la entrada en el directorio padre
                if(mi_write_f(*p_inodo_dir, &entrada, inodo_dir.tamEnBytesLog, sizeof(struct entrada)) == FALLO)
                {
                    if(entrada.ninodo != -1)
                    {
                        liberar_inodo(entrada.ninodo);

#if DEBUG7
                        printf("[buscar_entrada()-> liberado inodo %i, reservado a %s\n]", num_entrada_inodo, inicial);
#endif
                    }
                    return FALLO;
                }
            }
        }
            
    }   
    //Si hemos llegado al final del camino (No estoy seguro)
    if(!strcmp(final, "/") || !strcmp(final, ""))
    {
        if((num_entrada_inodo < cant_entradas_inodo)&&(reservar == 1))
        {
            return ERROR_ENTRADA_YA_EXISTENTE;
        }

        p_inodo = num_entrada_inodo;
        p_entrada = entrada.ninodo;

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
