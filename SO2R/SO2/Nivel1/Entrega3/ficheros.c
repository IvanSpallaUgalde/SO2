/**
 * Equipo: Alea iacta est
 * Alzamora Llabata, Pere
 * Arenas Afán de Rivera, Francisco
 * Reyes Gómez, Andrea Valentina
 */

#include "directorios.h"

/*
 * Función: mi_write_f()
 * Escribe el contenido procedente de un buffer de memoria, buf_original, de tamaño nbytes, 
 * en un fichero/directorio (correspondiente al inodo pasado como argumento, ninodo)
 * Input        ninodo: número de inodo en el cual escribir
 *              *buf_original: buffer de datos a escribir original
 *              offset: posición lógica
 *              nbytes: numero de bytes a escribir
 * Output       numero de bytes escritos
 *             -1: error de ejecución
 */
int mi_write_f(unsigned int ninodo, const void *buf_original, unsigned int offset, unsigned int nbytes){
    
    int primerBL, ultimoBL, desp1, desp2, nbfisico, nbytes_escritos;
    struct inodo my_inodo;
    unsigned char buf_bloque[BLOCKSIZE];

    if (leer_inodo(ninodo, &my_inodo) < 0)
    {
        return 0; // No se escribió ningún byte
    }

    if ((my_inodo.permisos & 2) == 2)
    {                                                 //Comprobamos que tenemos permiso de escritura
        primerBL = offset / BLOCKSIZE;                //Primer bloque lógico
        ultimoBL = (offset + nbytes - 1) / BLOCKSIZE; //Último bloque lógico

        //Calculamos los desplazamientos
        desp1 = offset % BLOCKSIZE;
        desp2 = (offset + nbytes - 1) % BLOCKSIZE;

        //Caso 1: el buffer cabe en un solo bloque
        if (primerBL == ultimoBL)
        {
            mi_waitSem();
            nbfisico = traducir_bloque_inodo(ninodo, primerBL, 1);
            mi_signalSem();
            if (bread(nbfisico, buf_bloque) < 0)
            {
                fprintf(stderr, "Error de lectura de bloque\n");
                return 0;
            }
            memcpy(buf_bloque + desp1, buf_original, nbytes);
            if ((nbytes_escritos = bwrite(nbfisico, buf_bloque)) < 0)
            {
                fprintf(stderr, "Error de escritura de bloque\n");
                return 0;
            }
        }
        else
        { //Caso 2: la operación de escritura efecte a más de un bloque

            //Primer bloque: preservar el contenido original del bloque virtual y sobreescribir
            mi_waitSem();
            nbfisico = traducir_bloque_inodo(ninodo, primerBL, 1);
            mi_signalSem();
            if (bread(nbfisico, buf_bloque) < 0)
            {
                fprintf(stderr, "Error de lectura de bloque\n");
                return 0;
            }
            int desp1 = offset % BLOCKSIZE;
            memcpy(buf_bloque + desp1, buf_original, BLOCKSIZE - desp1);

            if (bwrite(nbfisico, buf_bloque) < 0)
            {
                fprintf(stderr, "Error de escritura de bloque\n");
                return 0;
            }

            mi_waitSem();
            //Bloques intermedios: no hay que preservar datos
            for (int i = primerBL + 1; i < ultimoBL; i++)
            {
                nbfisico = traducir_bloque_inodo(ninodo, i, 1);
                if (bwrite(nbfisico, buf_original + (BLOCKSIZE - desp1) + (i - primerBL - 1) * BLOCKSIZE) < 0)
                {
                    fprintf(stderr, "Error de escritura de bloque\n");
                    mi_signalSem();
                    return 0;
                }
            }
            mi_signalSem();


            //Bloque final: preservar la parte restante
            mi_waitSem();
            nbfisico = traducir_bloque_inodo(ninodo, ultimoBL, 1);
            mi_signalSem();            
            if (bread(nbfisico, buf_bloque) < 0)
            {
                fprintf(stderr, "Error de lectura");
                return 0;
            }
            desp2 = (offset + nbytes - 1) % BLOCKSIZE;
            memcpy(buf_bloque, buf_original + (nbytes - desp2 - 1), desp2 + 1);
            if ((nbytes_escritos = bwrite(nbfisico, buf_bloque)) < 0)
            {
                fprintf(stderr, "Error de escritura");
                return 0;
            }
        }
                
        mi_waitSem();
        if (leer_inodo(ninodo, &my_inodo) < 0)
        {
            mi_signalSem();
            return 0;
        }
        //Actualizamos la metainformación del inodo

        //Si hemos escrito más allá del final del fichero
        if (offset + nbytes >= my_inodo.tamEnBytesLog)
        {
            my_inodo.tamEnBytesLog = offset + nbytes;
            my_inodo.ctime = time(NULL);
        }

        my_inodo.mtime = time(NULL);

        if (escribir_inodo(ninodo, my_inodo) < 0)
        {
            mi_signalSem();
            return 0;
        }
        mi_signalSem();
        return nbytes;
    }
    else
    { //El inodo no tiene permisos de escritura
        fprintf(stderr, "El fichero no esta disponible en modo escritura\n");
        return 0;
    }
}

/*
 * Función: mi_read_f()
 * Lee la informacion de un fichero/directorio (ninodo) y la almacena en un
 * buffer de memoria
 * Input        ninodo: número de inodo en el cual leer
 *              *buf_original: buffer de datos a leer original
 *              offset: posición lógica
 *              nbytes: numero de bytes a leer
 * Output       numero de bytes leidos
 *             -1: error de ejecución
 */

int mi_read_f(unsigned int ninodo, void *buf_original, unsigned int offset, unsigned int nbytes)
{
    int primerBL, ultimoBL, desp1, desp2, nbfisico, nbytes_leidos;
    struct inodo my_inodo;
    unsigned char buf_bloque[BLOCKSIZE];

    if (leer_inodo(ninodo, &my_inodo) < 0)
    {
        return -1;
    }

    nbytes_leidos = 0;

    if ((my_inodo.permisos & 4) == 4)
    {
        //Comprobamos que el offset esté dentro del tamaño en bytes lógicos del inodo
        if (offset >= my_inodo.tamEnBytesLog)
        {
            //fprintf(stderr,"El offset es mayor al tamaño de bytes del inodo\n");
            return nbytes_leidos;
        }
        //Si pretende leer más allá de EOF leemos solo hasta el EOF
        if ((offset + nbytes) >= my_inodo.tamEnBytesLog)
        {
            nbytes = my_inodo.tamEnBytesLog - offset;
        }

        primerBL = offset / BLOCKSIZE;                //Primer bloque lógico
        ultimoBL = (offset + nbytes - 1) / BLOCKSIZE; //Último bloque lógico

        //Calculamos los desplazamientos
        desp1 = offset % BLOCKSIZE;
        desp2 = (offset + nbytes - 1) % BLOCKSIZE;

        //Caso 1: el buffer cabe en un solo bloque
        if (primerBL == ultimoBL)
        {
            nbfisico = traducir_bloque_inodo(ninodo, primerBL, 0);
            if (nbfisico > 0)
            {
                if (bread(nbfisico, buf_bloque) < 0)
                {
                    fprintf(stderr, "Lectura incorrecta del bloque fisico\n");
                    return -1;
                }
                memcpy(buf_original, buf_bloque + desp1, nbytes);
            }
            nbytes_leidos = nbytes;
        }
        else
        { //Caso 2: la operación de lectura efecte a más de un bloque
            //Primer bloque: preservar el contenido original del bloque virtual y sobreescribir

            nbfisico = traducir_bloque_inodo(ninodo, primerBL, 0);

            if (nbfisico > 0)
            {
                if (bread(nbfisico, buf_bloque) < 0)
                {
                    fprintf(stderr, "Lectura incorrecta\n");
                    return -1;
                }
                memcpy(buf_original, buf_bloque + desp1, BLOCKSIZE - desp1);
            }
            nbytes_leidos = (BLOCKSIZE - desp1);

            //Bloques intermedios: no hay que preservar datos
            for (int i = primerBL + 1; i < ultimoBL; i++)
            {
                nbfisico = traducir_bloque_inodo(ninodo, i, 0);
                if (nbfisico > 0)
                {
                    if (bread(nbfisico, buf_bloque) < 0)
                    {
                        fprintf(stderr, "Error de lectura\n");
                        return -1;
                    }
                    memcpy(buf_original + nbytes_leidos, buf_bloque, BLOCKSIZE);
                }
                nbytes_leidos += BLOCKSIZE;
            }

            //Bloque final: preservar la parte restante
            nbfisico = traducir_bloque_inodo(ninodo, ultimoBL, 0);

            if (nbfisico > 0)
            {

                if (bread(nbfisico, buf_bloque) < 0)
                {
                    fprintf(stderr, "Error de lectura\n");
                    return -1;
                }
                memcpy(buf_original + nbytes_leidos, buf_bloque, desp2 + 1);
            }
            nbytes_leidos += desp2 + 1;
        }

        mi_waitSem();
        //Actualizamos metadatos
        if (leer_inodo(ninodo, &my_inodo) < 0)
        {
            mi_signalSem();
            return -1;
        }
        my_inodo.atime = time(NULL);
        if (escribir_inodo(ninodo, my_inodo) < 0)
        {
            mi_signalSem();
            return -1;
        }
        mi_signalSem();
        return nbytes_leidos;
    }
    else
    { //El inodo no tiene permisos de lectura
        fprintf(stderr, "El fichero no esta disponible en modo lectura");
        return -1;
    }
}

/*
 * Función: mi_stat_f()
 * Devuelve la metainformación de un fichero/directorio
 * Input        ninodo: número de inodo en el array de inodos
 *              *p_stat: puntero STAT donde se guardaŕa la información
 * Output        0: ejecución correcta
 */

int mi_stat_f(unsigned int ninodo, struct STAT *p_stat)
{
    struct inodo my_inodo;

    if (leer_inodo(ninodo, &my_inodo) < 0)
    {
        return -1;
    }

    p_stat->atime = my_inodo.atime;
    p_stat->ctime = my_inodo.ctime;
    p_stat->mtime = my_inodo.mtime;
    p_stat->nlinks = my_inodo.nlinks;
    p_stat->numBloquesOcupados = my_inodo.numBloquesOcupados;
    p_stat->permisos = my_inodo.permisos;
    p_stat->tamEnBytesLog = my_inodo.tamEnBytesLog;
    p_stat->tipo = my_inodo.tipo;

    return 0;
}

/*
 * Función: mi_chmod_f()
 * Input        ninodo: número de inodo en el array de inodos
 *              permisos
 * Output        0: ejecución correcta
 */
int mi_chmod_f(unsigned int ninodo, unsigned char permisos)
{
    mi_waitSem();
    struct inodo my_inodo;

    if (leer_inodo(ninodo, &my_inodo) < 0)
    {
        fprintf(stderr, "Error: No se pudo leer inodo para cambiar sus permisos");
        mi_signalSem();
        return -1;
    }
    my_inodo.permisos = permisos;
    my_inodo.ctime = time(NULL);
    if (escribir_inodo(ninodo, my_inodo) < 0)
    {
        fprintf(stderr, "Error: No se pudo leer inodo para cambiar sus permisos");
        mi_signalSem();
        return -1;
    }
    mi_signalSem();
    return 0;
}

// ------- NIVEL 6 ------- //

/*
 * Función: mi_truncar_f()
 * Trunca un fichero/directorio a los bytes indicados como nbytes, 
 * liberando los bloques necesarios.
 * Input        ninodo: número de inodo en el array de inodos
 *              nbytes: cantidad de bytes a truncar
 * Output        0: ejecución correcta
 *              -1: error de ejecución
 */
int mi_truncar_f(unsigned int ninodo, unsigned int nbytes)
{
    int primerBL, liberados;
    struct inodo my_inodo;
    if (leer_inodo(ninodo, &my_inodo) < 0)
        return -1;

    //Comprobar que el inodo tenga permisos de escritura:
    if ((my_inodo.permisos & 2) == 2)
    {
        if (nbytes > my_inodo.tamEnBytesLog)
        {
            fprintf(stderr, "Error: El tamaño del fichero es menor a la cantidad a truncar");
            return -1;
        }
        //Nos basamos en la función liberar_bloques_inodo(), para saber que
        // nº de bloque lógico le hemos de pasar como primer bloque lógico a liberar
        if (nbytes % BLOCKSIZE == 0)
        {
            primerBL = nbytes / BLOCKSIZE;
        }
        else
        {
            primerBL = nbytes / BLOCKSIZE + 1;
        }

        liberados = liberar_bloques_inodo(primerBL, &my_inodo);
        //Actualizaciones
        my_inodo.mtime = time(NULL);
        my_inodo.ctime = time(NULL);
        my_inodo.tamEnBytesLog = nbytes;
        my_inodo.numBloquesOcupados = my_inodo.numBloquesOcupados - liberados;

        if (escribir_inodo(ninodo, my_inodo) < 0)
        { //escribimos el inodo
            return -1;
        }
        return liberados;
    }
    else
    {
        fprintf(stderr, "El inodo no tiene permisos de escritura");
        return -1;
    }
}