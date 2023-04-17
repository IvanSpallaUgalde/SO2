#include "bloques.h"

#define BLOCKSIZE 1024 // bytes

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