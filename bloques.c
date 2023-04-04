#include <stdio.h>  //printf(), fprintf(), stderr, stdout, stdin
#include <fcntl.h> //O_WRONLY, O_CREAT, O_TRUNC
#include <sys/stat.h> //S_IRUSR, S_IWUSR
#include <stdlib.h>  //exit(), EXIT_SUCCESS, EXIT_FAILURE, atoi()
#include <unistd.h> // SEEK_SET, read(), write(), open(), close(), lseek()
#include <errno.h>  //errno
#include <string.h> // strerror()
#include "bloques.h"

#define BLOCKSIZE 1024 // bytes


#define EXITO 0 //gestión errores
#define FALLO -1 //gestión errores

static int descriptor = 0;


int bmount(const char *camino)
{    
    umask(000);
    if (descriptor > 0)
    {
        close(descriptor);
    }
    descriptor = open(camino, O_RDWR|O_CREAT, 0666);
    if (descriptor == -1){
        return FALLO;  // Error
    }else{
        return descriptor;    
    }
}

/* función para desmontar el dispositivo virtual*/
int bumount(){
    if (descriptor == -1)
    {
        return FALLO;
    }
    else{
        return close(descriptor);
    }

}

/* función para escribir un bloque*/
int bwrite(unsigned int nbloque, const void *buf){
    //Posicionar el puntero en el offset
    lseek(descriptor,nbloque*BLOCKSIZE,SEEK_SET);
    //Escritura
    if (write(descriptor,&buf,BLOCKSIZE)<0){
        return FALLO;
    }
    return BLOCKSIZE;

}

/* función para leer un bloque*/
int bread(unsigned int nbloque, void *buf){
    //Posicionar el puntero en el offset
    lseek(descriptor,nbloque*BLOCKSIZE,SEEK_SET);
    //Lectura
    if(read(descriptor, &buf,BLOCKSIZE)<0){
        FALLO;
    }
    return BLOCKSIZE;
}