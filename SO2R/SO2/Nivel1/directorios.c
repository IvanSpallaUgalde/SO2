/**
 * Equipo: Alea iacta est
 * Alzamora Llabata, Pere
 * Arenas Afán de Rivera, Francisco
 * Reyes Gómez, Andrea Valentina
 */

#include "directorios.h"

// ---- Variable global del nivel 9 ---- //
static struct UltimaEntrada UltimaEntradaEscritura;
static struct UltimaEntrada UltimaEntradaLectura;

/*
 * Función: extraer_camino()
 * Extrae los elementos individuales de la ruta
 * Input         camino: ruta al archivo que se quiere abrir
 *              inicial: nombre del directorio o fichero
 *                 tipo: 0 = directorio
 *                       1 = fichero
 * Output            -1: el camino está vacío o no empieza por /
 */
int extraer_camino(const char *camino, char *inicial, char *final, char *tipo){
    char path[strlen(camino)];
	int  segunda_barra = 0;
	
    strcpy(path,camino);
	if(path[0] != '/'){ //Mostramos error si el camino no empieza por '/'
		//fprintf(stderr, "El camino debe de comenzar por /\n");
		return -1;
	}else{
		//Comprobamos que haya un segundo caracter '/'
		for(int i = 1; i < strlen(path); i++){ //Empieza en uno porque nos saltamos el primer carácter que es '/'
			if(path[i] == '/'){
				strncpy(inicial,path+1,sizeof(char) * i - 1); // Se guarda el resto del camino despues de la primera barra
				strcpy(final,path + i);  
				segunda_barra = 1;
				*tipo='d';
				return segunda_barra;
			}
		}

		if(segunda_barra==0){
			strncpy(inicial,path+1,sizeof(char)*strlen(path)-1);
			
			*tipo = 'f'; //indicamos que es un fichero
			return segunda_barra;
		}
		return -1;
	}	
	
}


/*
 * Función: buscar_entrada()
 * Busca una determinada entrada entre las entradas del inodo correspondiente
 * a su directorio padre.
 * Input    camino_parcial: 
 *             p_inodo_dir:
 *                 p_inodo: 
 *               p_entrada:
 *                reservar:
 *                permisos:
 * Output               -1: error de ejecucion
 */
int buscar_entrada(const char *camino_parcial, unsigned int *p_inodo_dir, 
unsigned int *p_inodo, unsigned int *p_entrada, char reservar, unsigned char permisos){
    struct entrada entrada;
    struct inodo inodo_dir;
    struct superbloque SB;
    char inicial[sizeof(entrada.nombre)];
    char final[strlen(camino_parcial)];
    char tipo;
    int cant_entradas_inodo, num_entrada_inodo;

    bread(posSB,&SB);

    if(strcmp(camino_parcial,"/")==0){
		*p_inodo = SB.posInodoRaiz; // La raiz siempre esta asociada al inodo 0
		*p_entrada = 0;
		return 0;
    }
	memset(inicial,0,sizeof(entrada.nombre));                               // Inicializamos el contenido de los buffer
    memset(final,0,strlen(camino_parcial)+1);
	
    if (extraer_camino(camino_parcial, inicial, final, &tipo)<0){
		return ERROR_CAMINO_INCORRECTO;
    }
	//fprintf(stderr,"[buscar_entrada()→ inicial: %s, final: %s, reservar: %d]\n",inicial, final, reservar);
    //Buscamos la entrada cuyo nombre se encuentra en inicial
    if(leer_inodo(*p_inodo_dir,&inodo_dir)<0){
		return -1;
	}
    if((inodo_dir.permisos & 4)!=4){ // El inodo no tiene permisos de lectura
		//fprintf (stderr, "[buscar_entrada()→ El inodo %d no tiene permisos de lectura]\n", *p_inodo_dir);
		return ERROR_PERMISO_LECTURA;
    }

	// El buffer de lectura puede ser un struct tipo entrada
	// o bien un array de las entradas que caben en un bloque
	cant_entradas_inodo = inodo_dir.tamEnBytesLog/sizeof(struct entrada);
	num_entrada_inodo = 0;
	memset(entrada.nombre,0,sizeof(entrada.nombre)); // Inicializar buffer de lectura con 0s
	if(cant_entradas_inodo>0){
		
		if(mi_read_f(*p_inodo_dir,&entrada,0,sizeof(struct entrada))<0){ // Leer entrada
			return -1;
		} 
		while((num_entrada_inodo < cant_entradas_inodo) && (strcmp(inicial,entrada.nombre)!=0)){
			num_entrada_inodo++;
			memset(entrada.nombre,0,sizeof(entrada.nombre)); // Inicializar buffer de lectura con 0s
			if(mi_read_f(*p_inodo_dir,&entrada,num_entrada_inodo*sizeof(struct entrada),sizeof(struct entrada))<0){ // Leer siguiente entrada
				return -1;
			}

		}
	}

	if (strcmp(inicial,entrada.nombre)!=0 && (num_entrada_inodo==cant_entradas_inodo)){ // La entrada no existe
		switch (reservar){
			case 0: // Modo consulta. Como no existe, retornamos error
				return ERROR_NO_EXISTE_ENTRADA_CONSULTA;
            
			case 1: // Modo escritura
				// Creamos la entrada en el directorio referenciado por *p_inodo_dir
				// Si es fichero no permitir escritura
				if(inodo_dir.tipo == 'f'){
					return ERROR_NO_SE_PUEDE_CREAR_ENTRADA_EN_UN_FICHERO;
				}
				// Si es directorio comprobamos que tiene permiso de escritura
				if((inodo_dir.permisos & 2) != 2){ // No tiene permisos de escritura
					//fprintf (stderr, "[buscar_entrada()→ El inodo %d no tiene permisos de escritura]\n", num_entrada_inodo);
					return ERROR_PERMISO_ESCRITURA;
				}else{
					strcpy(entrada.nombre,inicial);
					if(tipo == 'd'){
						if(strcmp(final,"/")==0){
							entrada.ninodo = reservar_inodo('d',permisos); // Reservar un inodo como directorio  asignarlo a la entrada
							//fprintf (stderr, "[buscar_entrada()→ reservado inodo %d tipo %c con permisos %d para %s]\n", entrada.ninodo, tipo, permisos, inicial);
						}else{ // Cuelgan más directorios o ficheros
							return ERROR_NO_EXISTE_DIRECTORIO_INTERMEDIO;
						}
					}else{ // Es un fichero
						entrada.ninodo = reservar_inodo('f',permisos); // Reservar un inodo como fichero y asignarlo a la entrada
						//fprintf (stderr, "[buscar_entrada()→ reservado inodo %d tipo %c con permisos %d para %s]\n", entrada.ninodo, tipo, permisos, inicial);
					}
					if (mi_write_f(*p_inodo_dir,&entrada, num_entrada_inodo*sizeof(struct entrada),sizeof(struct entrada))<0){ // REVISAR ESTA ESCRITURA DE ENTRADA #################################
						if(entrada.ninodo != 1){
							liberar_inodo(entrada.ninodo);
							//fprintf(stderr,"[buscar_entrada()-> liberado inodo %i, reservado a %s]\n", num_entrada_inodo, inicial);
						}else{
							return -1;
						}
					}
					//fprintf(stderr,"[buscar_entrada()-> creada entrada: %s, %d]\n",entrada.nombre, entrada.ninodo);
				}
		}
	}
	if ((strcmp(final,"")==0) || (strcmp(final,"/")==0)){
		if ((num_entrada_inodo < cant_entradas_inodo) && (reservar==1)){
			// Modo escritura y la entrada ya existe
			return ERROR_ENTRADA_YA_EXISTENTE;
		}
		// Se corta la recursividad
		*p_inodo = entrada.ninodo;
		*p_entrada = num_entrada_inodo;
		return 0;
	}else{
		*p_inodo_dir = entrada.ninodo;
		return buscar_entrada(final, p_inodo_dir, p_inodo, p_entrada, reservar, permisos);
	}
	return 0;
}

/*
 * Función: mostrar_error_buscar_entrada()
 * Función auxiliar para imprimir los mensajes de los diferentes errores
 * Input		error: nombre del error
 * Output		void
 */
void mostrar_error_buscar_entrada(int error) {
   switch (error) {
   case -1: fprintf(stderr, "Error: Camino incorrecto.\n"); break;
   case -2: fprintf(stderr, "Error: Permiso denegado de lectura.\n"); break;
   case -3: fprintf(stderr, "Error: No existe el archivo o el directorio.\n"); break;
   case -4: fprintf(stderr, "Error: No existe algún directorio intermedio.\n"); break;
   case -5: fprintf(stderr, "Error: Permiso denegado de escritura.\n"); break;
   case -6: fprintf(stderr, "Error: El archivo ya existe.\n"); break;
   case -7: fprintf(stderr, "Error: No es un directorio.\n"); break;
   }
}

/*
 * Función: mi_creat()
 * Crea un fichero/directorio y su entrada de directorio 
 * Input:		camino:
 * 				permisos:
 * Output:		-1:	Fail
 * 				0: Success
 */
int mi_creat(const char *camino, unsigned char permisos){
	mi_waitSem();
	unsigned int p_inodo=0, p_entrada=0, p_inodo_dir = 0;
	int error;
	if ((error = buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada, 1, permisos)) < 0) {
    	// fprintf(stderr,"mi_creat: No se pudo crear el fichero/directorio\n");
		mostrar_error_buscar_entrada(error);
		mi_signalSem();
		return -1;
  	}else {
		mi_signalSem();
		return 0;
	}
}

/*
 * Función: mi_dir()
 * Pone el contenido del directorio en un buffer que separa cada nombre por |
 * Input:		camino:
 * 				buffer: buffer vacío donde se guardarán los contenidos separados por '|'
 * Output:		número de entradas
 */
int mi_dir(const char *camino, char *buffer){
	struct inodo inodo;
	struct superbloque SB;
	struct entrada entrada;
	struct tm *tm; 
	bread(posSB,&SB);
	unsigned int p_entrada = 0;
	unsigned int p_inodo_dir = SB.posInodoRaiz;
	unsigned int p_inodo = SB.posInodoRaiz;
	
	char tmp[100], tam[20];
	int error, cant_entradas;
	
	// A lo mejor este buscar_entrada deberia tener permisos de lectura
	if ((error = buscar_entrada(camino,&p_inodo_dir, &p_inodo, &p_entrada,0,0)) < 0) {// Buscamos la entrada correspondiente a camino
		mostrar_error_buscar_entrada(error);
		return -1;
	}else{
		leer_inodo(p_inodo, &inodo);
		cant_entradas = inodo.tamEnBytesLog/sizeof(struct entrada);
		if(inodo.tipo != 'd'){ // Comprobamos que el inodo sea tipo directorio
			fprintf(stderr,"El inodo no es de tipo directorio\n");
			return -7;
		}
		if(inodo.tipo != 'd' && inodo.permisos & 4){
		//if((inodo.permisos & 4)!=4){ //Comprobamos que el inodo tenga permisos de lectura
			fprintf(stderr,"El inodo no tiene permisos de lectura\n");
			return -2;
		}

		//p_entrada = SB.posInodoRaiz;
		for(int i = 0; i < cant_entradas; i++){ //Recorremos todas las entradas
			//memset(entrada.nombre,0,sizeof(entrada.nombre)); // Inicializar buffer de lectura con 0s
			if(mi_read_f(p_inodo,&entrada,i*sizeof(struct entrada),sizeof(struct entrada))<0){ // Leer entrada
				return -1;
			} 
			leer_inodo(entrada.ninodo, &inodo); // Leemos el inodo asociado a cada entrada

			// Añadimos el tipo
			if (inodo.tipo == 'd') strcat(buffer, "d"); else strcat(buffer, "f"); 
			strcat(buffer, "\t");

			// Añadimos los permisos
			if (inodo.permisos & 4) strcat(buffer, "r"); else strcat(buffer, "-");
			if (inodo.permisos & 2) strcat(buffer, "w"); else strcat(buffer, "-");
			if (inodo.permisos & 1) strcat(buffer, "x"); else strcat(buffer, "-");
			strcat(buffer, "\t");

			// Añadimos información del tiempo
			tm = localtime(&inodo.mtime);
			sprintf(tmp, "%d-%02d-%02d %02d:%02d:%02d", tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday, tm->tm_hour, tm->tm_min,  tm->tm_sec);
			strcat(buffer, tmp);
			strcat(buffer, "\t");
			
			// Añadimos el tamaño
			sprintf(tam, "%d", inodo.tamEnBytesLog);
			strcat(buffer, tam);
			strcat(buffer, "\t");

			// Añadimos el nombre
			strcat(buffer, entrada.nombre);
			strcat(buffer, "\n");
		}
		return cant_entradas;
	}
}

/*
 * Función: mi_chmod()
 * Cambia los permisos de un fichero/directorio 
 * Input:		camino:
 * 				permisos:
 * Output:		-1:	Fail
 * 				0: Success
 */
int mi_chmod(const char *camino, unsigned char permisos){
	unsigned int p_inodo=0, p_entrada=0, p_inodo_dir = 0;
	int error;
	if((error = buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada, 0, permisos))<0){
		// fprintf(stderr,"mi_chmod: No se pudo encontrar el fichero/directorio\n");
		mostrar_error_buscar_entrada(error);
		return -1;
	} else {
		mi_chmod_f(p_inodo,permisos);
		return 0;	
	}
}

/*
 * Función: mi_stat()
 * Obtiene el pinodo de una entrada
 * Input:		camino
 * 				p_stat
 * Output:	   -1: Fail
 * 				0: Success
 */
int mi_stat(const char *camino, struct STAT *p_stat){
	unsigned int p_inodo=0, p_entrada=0, p_inodo_dir = 0;
	int error;

	if((error = buscar_entrada(camino,&p_inodo_dir,&p_inodo,&p_entrada,0,p_stat->permisos))<0){
		mostrar_error_buscar_entrada(error);
		return -1;
	} else {
		mi_stat_f(p_inodo,p_stat);
		return 0;	
	}
}

// ---------------- NIVEL 9 ---------------- //

/* POSIBLE OPTIMIZACIÓN --> Convertir UltimaEntradaEscritura en una array, cola, pila... y guardar multiples valores. <-- TAMBIÉN SIRVE PARA mi_read()
 * Función: mi_write
 * Escribe contenido en un fichero
 * Input:	camino: Dirección del fichero en el que se escribirá
 * 			buf: Buffer con el contenido a escribir
 * 			offset: Offset en el que se escribirá
 * 			nbytes: Cantidad de bytes a escribir
 * 
 * Output:	Número de bytes escritos
 * 			-1 Si ha funcionado incorrectamente
 */
int mi_write(const char *camino, const void *buf, unsigned int offset, unsigned int nbytes){
	// LA SECCION CRITICA ESTA EN mi_write_f
	unsigned int p_inodo, p_entrada, p_inodo_dir;
	int error;
	if(strcmp(UltimaEntradaEscritura.camino,camino)==0){
		// fprintf(stderr,"[mi_write() → Utilizamos la caché de escritura en vez de llamar a buscar_entrada()]\n");
		p_inodo = UltimaEntradaEscritura.p_inodo;
	} else {
		p_inodo_dir = 0;
		if((error = buscar_entrada(camino,&p_inodo_dir,&p_inodo,&p_entrada,0,6))<0){
			// fprintf(stderr, "mi_write: No se pudo encontrar el fichero\n");
			mostrar_error_buscar_entrada(error);
			return -1;
		} else { // Se actualiza la cache
			//Modificar la optimizacion
			// fprintf(stderr,"[mi_write() → Actualizamos la caché de escritura]\n");
			strcpy(UltimaEntradaEscritura.camino,camino);
			UltimaEntradaEscritura.p_inodo=p_inodo;
		}
	}
	return mi_write_f(p_inodo,buf,offset,nbytes);
}

/*
 * Función: mi_read
 * Lee el contenido de un fichero y lo introduce en un buffer
 * Input:	camino: Dirección del fichero a leer
 * 			buf: Buffer en el que se escribirá el contenido
 * 			offset: Offset del contenido a leer
 * 			nbytes: Número de bytes a leer
 * Output: 	numero de bytes leidos
 *             -1: error de ejecución
 */ 
int mi_read(const char *camino, void *buf, unsigned int offset, unsigned int nbytes){
	unsigned int p_inodo, p_entrada, p_inodo_dir;	
	int error;
	if(strcmp(UltimaEntradaLectura.camino,camino)==0){
		// fprintf(stderr,"\n[mi_read() → Utilizamos la caché de lectura en vez de llamar a buscar_entrada()]\n");
		p_inodo = UltimaEntradaLectura.p_inodo;
	} else {
		p_inodo_dir = 0;
		if((error = buscar_entrada(camino,&p_inodo_dir,&p_inodo,&p_entrada,0,0))<0){
			// fprintf(stderr, "mi_read: Error al usar buscar_entrada()\n");
			mostrar_error_buscar_entrada(error);
			return -1;
		} else { // Se actualiza la cache
			strcpy(UltimaEntradaLectura.camino,camino);
			// fprintf(stderr,"[mi_read() → Actualizamos la caché de lectura]\n");
			UltimaEntradaLectura.p_inodo=p_inodo;
		}
	}
	return mi_read_f(p_inodo,buf,offset,nbytes);
}

/*
 * Función: mi_link
 * Crea el enlace de una entrada de directorio camino2 al inodo 
 * especificado por otra entrada de directorio camino1.
 * Input:	camino1: fichero con el que se quiere enlazar
 *			camino2: fichero a enlazar
 * Output:  0: ejecucion correcta
 *		   -1: error de ejecucion
 */ 
int mi_link(const char *camino1, const char *camino2){
	mi_waitSem();
	unsigned int p_inodo1=0, p_entrada1=0, p_inodo_dir1 = 0;
	unsigned int p_inodo2=0, p_entrada2=0, p_inodo_dir2 = 0;
	int error;
	struct entrada entry;
	struct inodo inodo;

	if((error = buscar_entrada(camino1,&p_inodo_dir1,&p_inodo1,&p_entrada1,0,6))<0){ 
		//Fichero 1 no existe
		mostrar_error_buscar_entrada(error);
		mi_signalSem();
		return -1;
	}

	if((error = buscar_entrada(camino2,&p_inodo_dir2,&p_inodo2,&p_entrada2,1,6))<0){ 
		//Fichero 2 ya existe
		mostrar_error_buscar_entrada(error);
		mi_signalSem();
		return -1;
	}

	if(mi_read_f(p_inodo_dir2,&entry,p_entrada2*sizeof(entry),sizeof(entry))<0){
		fprintf(stderr,"mi_link: error al ejecutar mi_read_f\n");
		mi_signalSem();
		return -1;
	}

	entry.ninodo = p_inodo1;
	
	if(mi_write_f(p_inodo_dir2,&entry,p_entrada2*sizeof(entry),sizeof(entry))<0){
		fprintf(stderr,"mi_link: error al ejecutar mi_write_f\n");
		mi_signalSem();
		return -1;
	}

	liberar_inodo(p_inodo2);


	if(leer_inodo(p_inodo1,&inodo)<0){
		fprintf(stderr,"mi_link: Error al leer inodo\n");
		mi_signalSem();
		return -1;
	}
	
	inodo.nlinks++;
	inodo.ctime = time(NULL);

	if(escribir_inodo(p_inodo1,inodo)<0){
		fprintf(stderr,"mi_link: Error al actualizar el inodo\n");
		mi_signalSem();
		return -1;
	}

	mi_signalSem();
	return 0;
}

/*
 * Función: mi_unlink
 * Borra la entrada de directorio especificada  y, en caso de que fuera el 
 * último enlace existente, borrar el propio fichero/directorio.

 * especificado por otra entrada de directorio camino1.
 * Input:	camino1: fichero con el que se quiere enlazar
 *			camino2: fichero a enlazar
 * Output:  0: ejecucion correcta
 *		   -1: error de ejecucion
 */ 
int mi_unlink(const char *camino){
	mi_waitSem();
	unsigned int p_inodo, p_entrada, p_inodo_dir = 0;	
	int error;
	struct inodo inodo1,inodo2;
	struct entrada entrada;
	
	error = buscar_entrada(camino,&p_inodo_dir,&p_inodo,&p_entrada,0,6);

	if(error<0){
		mostrar_error_buscar_entrada(error);
		mi_signalSem();
		return -1;
	}
	
	if(leer_inodo(p_inodo,&inodo2)<0){
		fprintf(stderr,"mi_unlink: Error al leer inodo\n");
		mi_signalSem();
		return -1;
	}

	if(inodo2.tipo=='d' && inodo2.tamEnBytesLog>0){
		fprintf(stderr,"mi_link: El directorio no esta vacío\n");
		mi_signalSem();
		return -1;
	} else {
		if(leer_inodo(p_inodo_dir,&inodo1)<0){
			fprintf(stderr,"mi_unlink: No se pudo leer inodo_dir\n");
			mi_signalSem();
			return -1;
		}
		int n_entradas = inodo1.tamEnBytesLog/sizeof(struct entrada);
		
		if(p_entrada!=n_entradas-1){
			if(mi_read_f(p_inodo_dir,&entrada,inodo1.tamEnBytesLog-sizeof(struct entrada),sizeof(struct entrada))<0){
				mi_signalSem();
				return -1;
			}
			if(mi_write_f(p_inodo_dir,&entrada,p_entrada*sizeof(struct entrada),sizeof(struct entrada))<0){
				mi_signalSem();
				return -1;
			}
		}

		mi_truncar_f(p_inodo_dir,inodo1.tamEnBytesLog-sizeof(struct entrada));
		
		if(leer_inodo(p_inodo,&inodo1)<0){
			fprintf(stderr,"mi_unlink: No se pudo leer inodo\n");
			mi_signalSem();
			return -1;
		}
		inodo1.nlinks--; // Se actualiza la cantidad de enlaces del inodo
		if(inodo1.nlinks==0){
			liberar_inodo(p_inodo);
		} else {
			inodo1.ctime = time(NULL);
			if(escribir_inodo(p_inodo,inodo1)<0){
				mi_signalSem();
				return -1;
			}
		}
	}
	mi_signalSem();
	return 0;
}
