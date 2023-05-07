
#include "ficheros_basico.h"
#include "directorios.h"

#define DEBUGSB 1
#define DEBUGSB1 0
#define DEBUGSB2 0
#define DEBUGSB3 0

unsigned char buffer_memoria[BLOCKSIZE];

int main(int argc,char **argv){

    if (argc != 3)
    {
        fprintf(stderr, "Error de sintaxis. Sintaxis correcta: ./mi_mkfs <nombre del fichero> <numero de bloques>\n");
        return FALLO;
    }

    //Parametros de argv
    char *path = argv[1];
    int nbloques = atoi(argv[2]);
    int ninodos = nbloques/4;

    //Init buff
    unsigned char buf[BLOCKSIZE];
    if (!memset(buf, 0, BLOCKSIZE))
    {
        return FALLO;
    }

    //Montaje
    if (bmount(path) == FALLO)
    {
        fprintf(stderr, "Error al montar el dispositivo \n");
        return FALLO;
    }
    
    //Escritura
    for (int i = 0; i < nbloques; i++)
    {
        if (bwrite(i, buf) == FALLO)
        {
            fprintf(stderr, "ERror al escribir en el dispositivo en la posicion %i\n", i);
            return FALLO;
        }
        
    }
    
    //Init metadatos
    if (initSB(nbloques, ninodos) == FALLO)
    {
        fprintf(stderr, "Error inicializando el superbloque");
        return FALLO;
    }

#if DEBUGSB1
    struct superbloque SB;
    bread(posSB, &SB);
    printf("\nDATOS DEL SUPERBLOQUE\n");
    printf("posPrimerBloqueMB = %d\n", SB.posPrimerBloqueMB);
    printf("posUltimoBloqueMB = %d\n", SB.posUltimobloqueMB); 
    printf("posPrimerBloqueAI = %d\n", SB.posPrimerBloqueAI); 
    printf("posUltimoBloqueAI = %d\n", SB.posUltimoBloqueAI);
    printf("posPrimerBloqueDatos = %d\n", SB.posPrimerBloqueDatos);
    printf("posUltimoBloqueDatos = %d\n", SB.posUltimoBloqueDatos);
    printf("posInodoRaiz = %d\n", SB.posInodoRaiz);
    printf("posPrimerInodoLibre = %d\n", SB.posPrimerInodoLibre);
    printf("cantBloquesLibres = %d\n", SB.cantBloquesLibres);
    printf("cantInodosLibres = %d\n", SB.cantInodosLibres);
    printf("totBloques = %d\n", SB.totBloques);
    printf("totInodos = %d\n", SB.totInodos);
#endif    

    if (initMB() == FALLO)
    {
        fprintf(stderr, "Error inicializando el mapa de bits");
        return FALLO;
    }

#if DEBUGSB2
    struct superbloque SB;
    bread(posSB, &SB);
    printf("\nDATOS DEL SUPERBLOQUE\n");
    printf("posPrimerBloqueMB = %d\n", SB.posPrimerBloqueMB);
    printf("posUltimoBloqueMB = %d\n", SB.posUltimobloqueMB); 
    printf("posPrimerBloqueAI = %d\n", SB.posPrimerBloqueAI); 
    printf("posUltimoBloqueAI = %d\n", SB.posUltimoBloqueAI);
    printf("posPrimerBloqueDatos = %d\n", SB.posPrimerBloqueDatos);
    printf("posUltimoBloqueDatos = %d\n", SB.posUltimoBloqueDatos);
    printf("posInodoRaiz = %d\n", SB.posInodoRaiz);
    printf("posPrimerInodoLibre = %d\n", SB.posPrimerInodoLibre);
    printf("cantBloquesLibres = %d\n", SB.cantBloquesLibres);
    printf("cantInodosLibres = %d\n", SB.cantInodosLibres);
    printf("totBloques = %d\n", SB.totBloques);
    printf("totInodos = %d\n", SB.totInodos);
#endif


    if(initAI() == FALLO)
    {
        fprintf(stderr, "Error inicializando la array de inodos");
        return FALLO;
    }

#if DEBUGSB3
    struct superbloque SB;
    bread(posSB, &SB);
    printf("\nDATOS DEL SUPERBLOQUE\n");
    printf("posPrimerBloqueMB = %d\n", SB.posPrimerBloqueMB);
    printf("posUltimoBloqueMB = %d\n", SB.posUltimobloqueMB); 
    printf("posPrimerBloqueAI = %d\n", SB.posPrimerBloqueAI); 
    printf("posUltimoBloqueAI = %d\n", SB.posUltimoBloqueAI);
    printf("posPrimerBloqueDatos = %d\n", SB.posPrimerBloqueDatos);
    printf("posUltimoBloqueDatos = %d\n", SB.posUltimoBloqueDatos);
    printf("posInodoRaiz = %d\n", SB.posInodoRaiz);
    printf("posPrimerInodoLibre = %d\n", SB.posPrimerInodoLibre);
    printf("cantBloquesLibres = %d\n", SB.cantBloquesLibres);
    printf("cantInodosLibres = %d\n", SB.cantInodosLibres);
    printf("totBloques = %d\n", SB.totBloques);
    printf("totInodos = %d\n", SB.totInodos);
#endif


    reservar_inodo('d', 7);
    
#if DEBUGSB
    struct superbloque SB;
    bread(posSB, &SB);
    printf("\nDATOS DEL SUPERBLOQUE\n");
    printf("posPrimerBloqueMB = %d\n", SB.posPrimerBloqueMB);
    printf("posUltimoBloqueMB = %d\n", SB.posUltimobloqueMB); 
    printf("posPrimerBloqueAI = %d\n", SB.posPrimerBloqueAI); 
    printf("posUltimoBloqueAI = %d\n", SB.posUltimoBloqueAI);
    printf("posPrimerBloqueDatos = %d\n", SB.posPrimerBloqueDatos);
    printf("posUltimoBloqueDatos = %d\n", SB.posUltimoBloqueDatos);
    printf("posInodoRaiz = %d\n", SB.posInodoRaiz);
    printf("posPrimerInodoLibre = %d\n", SB.posPrimerInodoLibre);
    printf("cantBloquesLibres = %d\n", SB.cantBloquesLibres);
    printf("cantInodosLibres = %d\n", SB.cantInodosLibres);
    printf("totBloques = %d\n", SB.totBloques);
    printf("totInodos = %d\n", SB.totInodos);
#endif

    //Cerramos el fichero
    if (bumount() == FALLO)
    {
        fprintf(stderr, "Error al desmontar el dispositivo \n");
        return FALLO;
    }
    return EXITO;
}