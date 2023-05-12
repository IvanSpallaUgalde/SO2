#include "ficheros_basico.h"
#include "directorios.h"

#define DEBUGSB 1   //Debug del superbloque
#define DEBUG7 1    //Debug del nivel 7

void mostrar_buscar_entrada(char *camino, char reservar)
{
    unsigned int p_inodo_dir = 0;
    unsigned int p_inodo = 0;
    unsigned int p_entrada = 0;
    int error;
    printf("\ncamino: $s, reservar: %d\n", camino, reservar);
    error = buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada, reservar, 6);

    if (error < 0)
    {
        mostrar_error_buscar_entrada(error);
    }
    printf("**********************************************************************\n");
    return;
}

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

#if DEBUG7
    //Mostrar creación directorios y errores
    mostrar_buscar_entrada("pruebas/", 1);                 //ERROR_CAMINO_INCORRECTO
    mostrar_buscar_entrada("/pruebas/", 0);                //ERROR_NO_EXISTE_ENTRADA_CONSULTA
    mostrar_buscar_entrada("/pruebas/docs/", 1);           //ERROR_NO_EXISTE_DIRECTORIO_INTERMEDIO
    mostrar_buscar_entrada("/pruebas/", 1);                // creamos /pruebas/
    mostrar_buscar_entrada("/pruebas/docs/", 1);           //creamos /pruebas/docs/
    mostrar_buscar_entrada("/pruebas/docs/doc1", 1);       //creamos /pruebas/docs/doc1
    mostrar_buscar_entrada("/pruebas/docs/doc1/doc11", 1); //ERROR_NO_SE_PUEDE_CREAR_ENTRADA_EN_UN_FICHERO
    mostrar_buscar_entrada("/pruebas/", 1);                //ERROR_ENTRADA_YA_EXISTENTE
    mostrar_buscar_entrada("/pruebas/docs/doc1", 0);       //consultamos /pruebas/docs/doc1
    mostrar_buscar_entrada("/pruebas/docs/doc1", 1);       //creamos /pruebas/docs/doc1
    mostrar_buscar_entrada("/pruebas/casos/", 1);          //creamos /pruebas/casos/
    mostrar_buscar_entrada("/pruebas/docs/doc2", 1);       //creamos /pruebas/docs/doc2
#endif

    if (bumount() == FALLO)
    {
        fprintf(stderr, "Error desmontando el dispositivo en leer_sf.c\n");
        return FALLO;
    }
    printf()
}
