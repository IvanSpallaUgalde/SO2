/**
 * Equipo: Aela iacta est
 * Alzamora Llabata, Pere
 * Arenas Afán de Rivera, Francisco
 * Reyes Gómez, Andrea Valentina
 */


#include <stdio.h>
#include "directorios.h"

void mostrar_buscar_entrada(char *camino, char reservar){
  unsigned int p_inodo_dir = 0;
  unsigned int p_inodo = 0;
  unsigned int p_entrada = 0;
  int error;
  printf("\ncamino: %s, reservar: %d\n", camino, reservar);
  if ((error = buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada, reservar, 6)) < 0) {
    mostrar_error_buscar_entrada(error);
  }
  printf("**********************************************************************\n");
  return;
}

int main(int argc, char **argv){
    bmount(argv[1]);
	struct superbloque buff;
    bread(posSB,&buff);

	printf ("\nDATOS DEL SUPERBLOQUE\n");
    printf("posPrimerBloqueMB: %d\n",buff.posPrimerBloqueMB);
    printf("posUltimoBloqueMB: %d\n",buff.posUltimoBloqueMB);
    printf("posPrimerBloqueAI: %d\n",buff.posPrimerBloqueAI);
    printf("posUltimoBloqueAI: %d\n",buff.posUltimoBloqueAI);
    printf("posPrimerBloqueDatos: %d\n",buff.posPrimerBloqueDatos);
    printf("posUltimoBloqueDatos: %d\n",buff.posUltimoBloqueDatos);
    printf("posInodoRaiz: %d\n",buff.posInodoRaiz);
    printf("posPrimerInodoLibre: %d\n",buff.posPrimerInodoLibre);
    printf("cantBloquesLibres %d\n",buff.cantBloquesLibres);
    printf("cantInodosLibres %d\n",buff.cantInodosLibres);
    printf("totBloques %d\n",buff.totBloques);
    printf("totInodos %d\n",buff.totInodos);
	
	/*
	//Mostrar creación directorios y errores
	mostrar_buscar_entrada("pruebas/", 1); //ERROR_CAMINO_INCORRECTO
	mostrar_buscar_entrada("/pruebas/", 0); //ERROR_NO_EXISTE_ENTRADA_CONSULTA
	mostrar_buscar_entrada("/pruebas/docs/", 1); //ERROR_NO_EXISTE_DIRECTORIO_INTERMEDIO
	mostrar_buscar_entrada("/pruebas/", 1); // creamos /pruebas/
	mostrar_buscar_entrada("/pruebas/docs/", 1); //creamos /pruebas/docs/
	mostrar_buscar_entrada("/pruebas/docs/doc1", 1); //creamos /pruebas/docs/doc1
	mostrar_buscar_entrada("/pruebas/docs/doc1/doc11", 1);  //ERROR_NO_SE_PUEDE_CREAR_ENTRADA_EN_UN_FICHERO
	mostrar_buscar_entrada("/pruebas/", 1); //ERROR_ENTRADA_YA_EXISTENTE
	mostrar_buscar_entrada("/pruebas/docs/doc1", 0); //consultamos /pruebas/docs/doc1
	mostrar_buscar_entrada("/pruebas/docs/doc1", 1); //creamos /pruebas/docs/doc1
	mostrar_buscar_entrada("/pruebas/casos/", 1); //creamos /pruebas/casos/
	mostrar_buscar_entrada("/pruebas/docs/doc2", 1); //creamos /pruebas/docs/doc2
	*/
	bumount();   
	return 0;

	// ------------------ TEST NIVEL 4 --------------------
	// struct tm *ts;
	// char atime[80];
	// char mtime[80];
	// char ctime[80];
	// int ninodo=1;
	// struct inodo inodo[INODOSIZE];

	// if (reservar_inodo('f',6)==-1) return -1;
	// traducir_bloque_inodo(1,8,1);
	// traducir_bloque_inodo(1,204,1);
	// traducir_bloque_inodo(1,30004,1);
	// traducir_bloque_inodo(1,400004,1);
	// traducir_bloque_inodo(1,468750,1);
	// if ((traducir_bloque_inodo(1,8,1))==-1) return -1;
	// if ((traducir_bloque_inodo(1,204,1))==-1) return -1;
	// if ((traducir_bloque_inodo(1,30004,1))==-1) return -1;
	// if ((traducir_bloque_inodo(1,400004,1))==-1) return -1;
	// if ((traducir_bloque_inodo(1,468750,1))==-1) return -1;
	// bread(posSB,&buff);
	// printf("\nDATOS DEL INODO RESERVADO 1\n");
	// if(leer_inodo(1,inodo)<0) return -1;
	// printf("tipo: %c\n", inodo->tipo);
	// printf("permisos: %d\n", inodo->permisos);
	// leer_inodo(ninodo, inodo);
	// ts = localtime(&inodo->atime);
	// strftime(atime, sizeof(atime), "%a %Y-%m-%d %H:%M:%S", ts);
	// ts = localtime(&inodo->mtime);
	// strftime(mtime, sizeof(mtime), "%a %Y-%m-%d %H:%M:%S", ts);
	// ts = localtime(&inodo->ctime);
	// strftime(ctime, sizeof(ctime), "%a %Y-%m-%d %H:%M:%S", ts);
	// printf("ID ninodo: %d \nATIME: %s \nMTIME: %s \nCTIME: %s\n",ninodo,atime,mtime,ctime);
	// printf("nlinks: %d\n", inodo->nlinks);
	// printf("tamEnBytesLog: %d\n", inodo->tamEnBytesLog);
	// printf("numBloquesOcupados: %d\n", inodo->numBloquesOcupados);
	// printf("SB.posPrimerInodoLibre = %d\n", buff.posPrimerInodoLibre);

	// ------------------ TEST NIVEL 3 --------------------
    // printf("Tamaño de bloque: %d, Tamaño de inodo: %d\n",BLOCKSIZE,INODOSIZE);
    
    // 
    // //struct inodo ibuff;
    
    // /*for (size_t i = 0; i < buff.totInodos; i++){
    //     aux = leer_inodo(i,&ibuff);
    //     if(aux<0) return -1;
    //     printf("%ld ",i);
        
    // }*/

    // printf("\nRESERVAMOS UN BLOQUE Y LUEGO LO LIBERAMOS\n");
    // aux = reservar_bloque();
    // printf("Se ha reservado el bloque fisico n° %d que era el 1º libre indicado por el MB \n",aux);
    // bread(posSB,&buff);
    // printf("SB.cantBloquesLibres = %d\n",buff.cantBloquesLibres);
    // printf("Liberamos el bloque\n");
    // liberar_bloque(aux);
    // bread(posSB,&buff);
    // printf("SB.cantBloquesLibres = %d\n",buff.cantBloquesLibres);
    
    // printf("MAPA DE BITS CON BLOQUES DE METADATOS OCUPADOS\n");
	// aux = leer_bit(0);
	// printf("leer_bit(%d)=%d\n",0,aux);

	// aux = leer_bit(buff.posPrimerBloqueMB);
	// printf("leer_bit(%d)=%d\n",buff.posPrimerBloqueMB,aux);

	// aux = leer_bit(buff.posUltimoBloqueMB);
	// printf("leer_bit(%d)=%d\n",buff.posUltimoBloqueMB,aux);

	// aux = leer_bit(buff.posPrimerBloqueAI);
	// printf("leer_bit(%d)=%d\n",buff.posPrimerBloqueAI,aux);

	// aux = leer_bit(buff.posUltimoBloqueAI);
	// printf("leer_bit(%d)=%d\n",buff.posUltimoBloqueAI,aux);

	// aux = leer_bit(buff.posPrimerBloqueDatos);
	// printf("leer_bit(%d)=%d\n",buff.posPrimerBloqueDatos,aux);

	// aux = leer_bit(buff.posUltimoBloqueDatos);
	// printf("leer_bit(%d)=%d\n",buff.posUltimoBloqueDatos,aux);

	// struct tm *ts;
	// char atime[80];
	// char mtime[80];
	// char ctime[80];
	
	// int ninodo=0;
	// struct inodo inodo[INODOSIZE];

	// printf("\nDATOS DEL DIRECTORIO RAIZ\n");
	// aux = leer_inodo(buff.posInodoRaiz,inodo);
	// printf("tipo: %c\n", inodo->tipo);
	// printf("permisos: %d\n", inodo->permisos);
	// leer_inodo(ninodo, inodo);
	// ts = localtime(&inodo->atime);
	// strftime(atime, sizeof(atime), "%a %Y-%m-%d %H:%M:%S", ts);
	// ts = localtime(&inodo->mtime);
	// strftime(mtime, sizeof(mtime), "%a %Y-%m-%d %H:%M:%S", ts);
	// ts = localtime(&inodo->ctime);
	// strftime(ctime, sizeof(ctime), "%a %Y-%m-%d %H:%M:%S", ts);
	// printf("ID ninodo: %d \nATIME: %s \nMTIME: %s \nCTIME: %s\n",ninodo,atime,mtime,ctime);
	// printf("nlinks: %d\n", inodo->nlinks);
	// printf("tamEnBytesLog: %d\n", inodo->tamEnBytesLog);
	// printf("numBloquesOcupados: %d\n", inodo->numBloquesOcupados);
	// return 0;
}