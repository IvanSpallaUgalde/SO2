#include "ficheros.h"

int mi_write_f(unsigned int ninodo, const void *buf_original, unsigned int offset, unsigned nbytes)
{
    int primerBL, ultimoBL, desp1, desp2, nbfisico, nbytes_escritos= 0, aux_nbytes_escritos;
    void *buf_aux=NULL;
    struct inodo inodo;
    unsigned char buf_bloque[BLOCKSIZE];

    if (leer_inodo(ninodo, &inodo) < 0)
    {
        return 0; // No se escribio ningun Byte
    }

    if ((inodo.permisos & 2) != 2)
    {
        fprintf(stderr, "Error: El inodo no tiene permiso de escritura");
        return FALLO;
    }

    // Obtencion del bloque logico
    primerBL = offset / BLOCKSIZE;
    ultimoBL = (offset + nbytes - 1) / BLOCKSIZE;

    // Desplazamientos del bloque logico
    desp1 = offset % BLOCKSIZE;
    desp2 = (offset + nbytes - 1) % BLOCKSIZE;

    nbfisico = traducir_bloque_inodo(&inodo, primerBL, 1);

    if (nbfisico == FALLO)
    {
        fprintf(stderr, "Error de obtencion del bloque fisico\n");
        return FALLO;
    }

    // Copiamos al buff aux el contenido de buf_original
    memcpy(buf_aux,buf_original,nbytes);
    
    // Lectura del bloque fisico
    if (bread(nbfisico, buf_aux) == FALLO)
    {
        fprintf(stderr, "Error de lectura del bloque fisico\n");
        return FALLO;
    }

    // Primer caso (primerBL == ultimoBL)

    if (primerBL == ultimoBL)
    {
        memcpy(buf_bloque + desp1, buf_original, nbytes);

        aux_nbytes_escritos = bwrite(nbfisico, buf_bloque);
        if (aux_nbytes_escritos == FALLO)
        {
            fprintf(stderr, "Error escribiendo el bloque fisico\n");
            return FALLO;
        }
        nbytes_escritos += nbytes;
    }

    else if (primerBL < ultimoBL)
    {
        memcpy(buf_bloque + desp1, buf_original, BLOCKSIZE - desp1);

        aux_nbytes_escritos = bwrite(nbfisico, buf_bloque);
        if (aux_nbytes_escritos == FALLO)
        {
            fprintf(stderr, "Error escribiendo el bloque fisico\n");
            return FALLO;
        }
        nbytes_escritos += aux_nbytes_escritos - desp1;

        for (int i = primerBL + 1; i < ultimoBL; i++)
        {
            nbfisico = traducir_bloque_inodo(&inodo, i, 1);
            if (nbfisico == -1)
            {
                fprintf(stderr, "Error obteniendo el bloque fisico\n");
                return FALLO;
            }

            aux_nbytes_escritos = bwrite(nbfisico, buf_original + (BLOCKSIZE - desp1) + (i - primerBL - 1) * BLOCKSIZE);
            if (aux_nbytes_escritos == FALLO)
            {
                fprintf(stderr, "Error escribiendo los bloques intermedios\n");
                return FALLO;
            }
            nbytes_escritos += aux_nbytes_escritos;
        }

        // Obtencion del bloque fisico
        nbfisico = traducir_bloque_inodo(&inodo, ultimoBL, 1);
        if (nbfisico == -1)
        {
            fprintf(stderr, "Error obteniendo el bloque fisico\n");
            return FALLO;
        }

        // Lectura del bloque fisico
        if (bread(nbfisico, buf_bloque) == FALLO)
        {
            fprintf(stderr, "Error de lectura del bloque fisico\n");
        }

        memcpy(buf_bloque, buf_original + (nbytes - desp2 - 1), desp2 + 1);

        aux_nbytes_escritos = bwrite(nbfisico, buf_bloque);
        if (aux_nbytes_escritos == FALLO)
        {
            fprintf(stderr, "Error de escritura del bloque\n");
            return FALLO;
        }

        nbytes_escritos += desp2 + 1;
    }

    //Lectura del inodo actualizado
    if (leer_inodo(ninodo, &inodo) == FALLO)
    {
        fprintf(stderr, "Error de lectura del inodo\n");
        return FALLO;
    }

    //Actualizacion de la informacion

    if (inodo.tamEnBytesLog < (nbytes + offset))
    {
        inodo.tamEnBytesLog = nbytes + offset;
        inodo.ctime = time(NULL);
    }
    
    inodo.mtime = time(NULL);

    if (escribir_inodo(ninodo, &inodo) == FALLO)
    {
        fprintf(stderr, "Error de escritura del inodo\n");
        return FALLO;   
    }

    //Comprobar que la escritura ha sido exitosa
    if (nbytes == nbytes_escritos)
    {
        return nbytes_escritos;
    }
    else
    {
        return FALLO;
    }
}

int mi_read_f(unsigned int ninodo, void *buf_original, unsigned int offset, unsigned int nbytes)
{
    int primerBL, ultimoBL, desp1, desp2, nbfisico, nbytes_leidos= 0, aux_nbytes_leidos;
    struct inodo inodo;
    unsigned char buf_bloque[BLOCKSIZE];

    if(leer_inodo(ninodo, &inodo) == FALLO)
    {
        fprintf(stderr, "Error del ectura del inodo\n");
        return FALLO;
    }

    if ((inodo.permisos & 4) != 4)
    {
        fprintf(stderr, "Error: no hay permiso de lectura del inodo");
        return FALLO;
    }

    if (offset >= inodo.tamEnBytesLog)
    {
        //No podemos leer nada
        return nbytes_leidos;
    }

    if((offset + nbytes) >= inodo.tamEnBytesLog)
    {
        nbytes = inodo.tamEnBytesLog - offset;
        //Leemos solo los bytes que podemos desde el offset hasta EOF
    }
    
    primerBL = offset / BLOCKSIZE;
    ultimoBL = (offset + nbytes - 1) / BLOCKSIZE;

    desp1 = offset % BLOCKSIZE;
    desp2 = (offset + nbytes - 1) % BLOCKSIZE;

    nbfisico = traducir_bloque_inodo(&inodo, primerBL, 0);

    if (primerBL == ultimoBL)
    {
        if (nbfisico != FALLO)
        {
            aux_nbytes_leidos = bread(nbfisico, buf_bloque);
            if (aux_nbytes_leidos == FALLO)
            {
                fprintf(stderr, "Error de lectura del bloque fisico\n");
                return FALLO;
            }
            memcpy(buf_original, buf_bloque + desp1, nbytes);
        }
        nbytes_leidos = nbytes;
    }
    else if(primerBL < ultimoBL)
    {
        if (nbfisico != FALLO)
        {
            aux_nbytes_leidos = bread(nbfisico, buf_bloque);
            if (aux_nbytes_leidos == FALLO)
            {
                fprintf(stderr, "Error de lectura del bloque fisico\n");
                return FALLO;
            }
            memcpy(buf_original, buf_bloque + desp1, BLOCKSIZE - desp1);
            
        }

        nbytes_leidos = BLOCKSIZE - desp1;
        
        for (int i = primerBL +1; i < ultimoBL; i++)
        {
            nbfisico = traducir_bloque_inodo(&inodo, i, 0);
            if (nbfisico != FALLO)
            {
                aux_nbytes_leidos = bread(nbfisico, buf_bloque);
                if (aux_nbytes_leidos == FALLO)
                {
                    fprintf(stderr, "Error de lectura del bloque fisico\n");
                    return FALLO;
                }
                memcpy(buf_original + (BLOCKSIZE - desp1) + (i - primerBL - 1) * BLOCKSIZE, buf_bloque, BLOCKSIZE);
            }

            nbytes_leidos += BLOCKSIZE;
        }
        
        if (nbfisico != FALLO)
        {
            aux_nbytes_leidos = bread(nbfisico, buf_bloque);
            if (aux_nbytes_leidos == FALLO)
            {
                fprintf(stderr, "Error de lectura del bloque fisico\n");
                return FALLO;
            }

            memcpy(buf_original + (nbytes - desp2 - 1), buf_bloque, desp2 + 1);
        }

        nbytes_leidos += desp2 + 1; 
        
    }
    
    if (leer_inodo(ninodo, &inodo) == FALLO)
    {
        fprintf(stderr, "Error de lectura del inodo\n");
        return FALLO;
    }

    inodo.atime = time(NULL);

    if(escribir_inodo(ninodo, &inodo) == FALLO)
    {
        fprintf(stderr, "Error de escritura del inodo\n");
        return FALLO;
    }

    if(nbytes == nbytes_leidos)
    {
        return nbytes_leidos;
    }
    else
    {
        return FALLO;
    }
}

int mi_stat_f(unsigned int ninodo, struct STAT *p_stat)
{
    struct inodo inodo;

    if(leer_inodo(ninodo, &inodo) == FALLO)
    {
        return FALLO;
    }

    p_stat->atime = inodo.atime;
    p_stat->ctime = inodo.ctime;
    p_stat->mtime = inodo.mtime;
    p_stat->nlinks = inodo.nlinks;
    p_stat->numBloquesOcupados = inodo.numBloquesOcupados;
    p_stat->permisos = inodo.permisos;
    p_stat->tamEnBytesLog = inodo.tamEnBytesLog;
    p_stat->tipo = inodo.tipo;

    return EXITO;
}

int my_chmod_f(unsigned int ninodo, unsigned char permisos)
{
    struct inodo inodo;
    if (leer_inodo(ninodo, &inodo) == FALLO)
    {
        fprintf(stderr, "Error de lectura de inodo\n");
        return FALLO;
    }

    inodo.permisos = permisos;
    inodo.ctime = time(NULL);

    if(escribir_inodo(ninodo, &inodo) == FALLO)
    {
        fprintf(stderr, "Error de escritura del inodo\n");
        return FALLO;
    }

    return EXITO;    
}

int mi_truncar_f(unsigned int ninodo, unsigned int nbytes)
{
    int primerBL, liberados;
    struct inodo inodo;

    if(leer_inodo(ninodo, &inodo) < 0)
    {
        fprintf(stderr, "Error leyendo el inodo en mi_truncar_f\n");
        return FALLO;
    }

    if ((inodo.permisos & 2) == 2)
    {
        if (nbytes > inodo.tamEnBytesLog)
        {
            fprintf(stderr, "Error: La cantidad de bytes a truncar es mayor que el tamaño del fichero\n");
            return FALLO;
        }
        
        if (nbytes % BLOCKSIZE == 0)
        {
            primerBL = nbytes / BLOCKSIZE;
        }
        else
        {
            primerBL = nbytes /BLOCKSIZE + 1;
        }
        
        liberados = liberar_bloques_inodo(primerBL, &inodo);

        inodo.mtime = time(NULL);
        inodo.ctime = time(NULL);
        inodo.tamEnBytesLog = nbytes;
        inodo.numBloquesOcupados = inodo.numBloquesOcupados - liberados;

        if (escribir_inodo(ninodo, &inodo) == FALLO)
        {
            fprintf(stderr, "Error escribiendo el inodo en mi_truncar_f\n");
            return FALLO;
        }

        return liberados;
    }
    else
    {
        fprintf(stderr, "Error: el inodo no tiene permiso de escritura\n");
        return FALLO;
    }
    
    
}