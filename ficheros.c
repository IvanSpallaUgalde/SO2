#include "ficheros.h"

int mi_write_f(unsigned int ninodo, const void *buf_original, unsigned int offset, unsigned nbytes)
{
    int primerBL, ultimoBL, desp1, desp2, nbfisico, nbytes_escritos, aux_nbytes_escritos;
    struct inodo inodo;
    unsigned char buf_bloque[BLOCKSIZE];

    if (leer_inodo(ninodo, &inodo) < 0)
    {
        return 0; // No se escribio ningun Byte
    }

    if ((inodo.permisos & 2) != 2)
    {
        fprintf(stderr,"Error: El inodo no tiene permiso de escritura");
        return FALLO;
    }

    //Obtencion del bloque logico
    primerBL = offset / BLOCKSIZE;
    ultimoBL = (offset + nbytes - 1) / BLOCKSIZE;

    //Desplazamientos del bloque logico
    desp1 = offset % BLOCKSIZE;
    desp2 = (offset + nbytes - 1) % BLOCKSIZE;

    nbfisico = traducir_bloque_inodo(ninodo, primerBL, 1);

    if (nbfisico == FALLO)
    {
        fprintf(stderr,"Error de obtencion del bloque fisico\n");
        return FALLO;
    }
    
    //Lectura del bloque fisico
    if (bread(nbfisico, buf_original) == FALLO)
    {
        fprintf(stderr, "Error de lectura del bloque fisico\n");
        return FALLO;
    }

    //Primer caso (primerBL == ultimoBL)

    if (primerBL == ultimoBL)
    {
        memcpy(buf_bloque + desp1, buf_original, nbytes);

        aux_nbytes_escritos = bwrite(nbfisico, buf_bloque);
        if (aux_nbytes_escritos == FALLO)
        {
            fprintf(stderr, "Error escribiendo el bloque fisico");
            return FALLO;
        }
        nbytes_escritos += nbytes;
    }
    

    else if(primerBL < ultimoBL)
    {
        memcpy(buf_bloque + desp1, buf_original, BLOCKSIZE - desp1);

        aux_nbytes_escritos = bwrite(nbfisico, buf_bloque);
        if (aux_nbytes_escritos == FALLO)
        {
            fprintf(stderr, "Error escribiendo el bloque fisico");
            return  FALLO;
        }
        nbytes_escritos += aux_nbytes_escritos - desp1;
        
        for(int i = primerBL + 1; i < ultimoBL; i++)
        {
            nbfisico = traducir_bloque_inodo(ninodo, i, 1);
            if (nbfisico == -1)
            {
                fprintf(stderr,"Error obteniendo el bloque fisico");
                return FALLO;
            }

            aux_nbytes_escritos = bwrite(nbfisico, buf_original + (BLOCKSIZE - desp1) + (i - primerBL - 1) * BLOCKSIZE);
            if (aux_nbytes_escritos == FALLO)
            {
                fprintf(stderr, "Error escribiendo los bloques intermedios");
                return FALLO;
            }
            nbytes_escritos += aux_nbytes_escritos;

            
        }
    }
}