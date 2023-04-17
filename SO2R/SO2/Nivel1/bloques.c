/**
 * Equipo: Alea iacta est
 * Alzamora Llabata, Pere
 * Arenas Afán de Rivera, Francisco
 * Reyes Gómez, Andrea Valentina
 */

#include "bloques.h"

static int descriptor = 0;
static sem_t *mutex;
static unsigned int inside_sc = 0;

/*
 * Función: bmount()
 * Monta el dispositivo virtual a partir del camino recibido como parámetro
 * Input        camino: Camino al archivo que se quiere abrir
 * Output       Error de lectura: -1
 *              Lectura correcta: Descriptor del archivo             
 */
int bmount(const char *camino){
	//Abrir archivo
    umask(000);
    if (descriptor > 0){
        close(descriptor);
    }
    descriptor = open(camino,O_RDWR|O_CREAT,0666); 
    if(descriptor==-1){ //Error
        fprintf(stderr,"Error %d: %s\n",errno,strerror(errno));
        return -1;
    }
    if (!mutex){
		mutex = initSem();
		if (mutex == SEM_FAILED){
			return -1;
		}
	}
    return descriptor;
    
}

/*
 * Función: bumount()
 * Desmonta el dispositivo virtual
 * Input        None
 * Output       Se ha desmontado correctamente:  0
 *              Se ha producido un error:       -1
 */
int bumount(){
    descriptor = close(descriptor);
    int check = close(descriptor);
    if(check == 0){
        deleteSem();
        return check;
    }
    fprintf(stderr,"Error %d: %s\n",errno,strerror(errno));
    return -1;
}

/*
 * Función: bwrite()
 * Escribe 1 bloque en el dispositivo virtual en el bloque especificado por nbloque
 * Input        nbloque:    Bloque físico a escribir
 *              buf:        Buffer
 * Output       Error:      -1
 *              Ejecución correcta: nº de bytes escritos
 */
int bwrite(unsigned int nbloque, const void *buf){
    int position;
    int n_bytes;

    position = nbloque * BLOCKSIZE;

    if(lseek(descriptor,position, SEEK_SET) == -1){
        fprintf(stderr,"Error %d: %s\n",errno,strerror(errno));
        return -1;
    }

    n_bytes = write(descriptor,buf, BLOCKSIZE);   

    if(n_bytes >0){
        return n_bytes;// Si la ejecucion es correcta devuelve BLOCKSIZE        
    }
    fprintf(stderr,"Error %d: %s\n",errno,strerror(errno));
    return -1;
    
}

/*
 * Función: bread()
 * Lee 1 bloque del dispositivo virtual. Corresponde al bloque físico especificado por nbloque
 * Input        nbloque:    Bloque físico a leer
 *              buf:        Buffer
 * Output:      Error:      -1
 *              Ejecución correcta: nº de bytes leídos
 */ 
int bread(unsigned int nbloque, void *buf){
	int check;
	if(lseek(descriptor,nbloque*BLOCKSIZE,SEEK_SET) == -1){
		fprintf(stderr,"Error %d: %s\n",errno,strerror(errno));
        return -1;
	}
    check = read(descriptor,buf,BLOCKSIZE);
    if (check<0){
        fprintf(stderr,"Error %d: %s\n",errno,strerror(errno));
        return -1;
    } else {
        return check;
    }
}

/*
 * Función mi_waitSem()
 * Llama a waitSem()
 */
void mi_waitSem(){
	if(!inside_sc){ // inside_sc == 0
		waitSem(mutex);
	}
	inside_sc++;
}

/*
 * Función mi_signalSem()
 * Llama a signalSem()
 */
void mi_signalSem(){
	inside_sc--;
	if(!inside_sc){
		signalSem(mutex);
	}
}