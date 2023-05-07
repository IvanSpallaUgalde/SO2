#include "ficheros_basico.h"
#include "directorios.h"

#define DEBUGSB 1   //Debug del superbloque

int main(int argc, char *argv[])
{
    
    if (argc != 2)
    {
        fprintf(stderr, "Sintaxis correcta: leer_sf <nombre_dispositivo>\n");
        return FALLO;
    }

    //Montamos el disco
    if (bmount(argv[1]) == FALLO)
    {
        fprintf(stderr, "Error al montar el dispositivo virtual.\n");
        return FALLO;
    }
    
    struct superbloque SB;
    if (bread(posSB, &SB) == FALLO)
    {
        fprintf(stderr, "Error de lectura del superbloque.\n");
        return FALLO;
    }

#if DEBUGSB
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

    if (bumount() == FALLO)
    {
        fprintf(stderr, "Error desmontando el dispositivo en leer_sf.c\n");
        return FALLO;
    }
    
}