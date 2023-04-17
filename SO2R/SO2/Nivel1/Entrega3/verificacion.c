/**
 * Equipo: Alea iacta est
 * Alzamora Llabata, Pere
 * Arenas Afán de Rivera, Francisco
 * Reyes Gómez, Andrea Valentina
 */

#include "verificacion.h"

int main(int argc, char const *argv[])
{
	// Verificamos la sintaxis
	if (argc == 3)
	{

		// Montamos el disco
		bmount(argv[1]);
		struct STAT stat;

		// Obtenemos el número de entradas a partir del stat del directorio de simulacion
		mi_stat(argv[2], &stat);
		int numEntradas = stat.tamEnBytesLog / sizeof(struct entrada);

		fprintf(stderr, "\ndir_sim: %s \n", argv[2]);
		fprintf(stderr, "numEntradas: %d    NUMPROCESOS: %d \n", numEntradas, NUMPROCESOS);
		// Si el nº de entradas != NUMPROCESOS entonces devolver -1 fsi
		if (NUMPROCESOS != numEntradas)
		{
			fprintf(stderr, "El numero de procesos es diferente al numero de entrada\n");
			return -1;
		}

        //Crear fichero "informe.txt"
		char fic[100];
		strcpy(fic,argv[2]);

		strcat(fic,"informe.txt");
		if(mi_creat(fic,6)<0){
			fprintf(stderr,"Error: no se pudo crear informe.txt\n");
			return -1;
		}

        //Leer directorios correspondientes a los procesos
		struct entrada entradas[NUMPROCESOS*sizeof(struct entrada)];
		if(mi_read(argv[2],entradas,0,NUMPROCESOS*sizeof(struct entrada))<0){
			fprintf(stderr,"Error: No se pudieron leer las entradas\n");
			return -1;
		}

		struct INFORMACION info;
		int infoPos = 0;
		char aux1[500];
		char aux2[500];

		for (int i=0;i<NUMPROCESOS;i++){
            //Leer entrada de directorio
			char *pid = strchr(entradas[i].nombre,'_');
			pid++;
            //Extraer el PID
			info.pid = atoi(pid);

			//Inicializar el resto de la estructura
			info.nEscrituras = 0;
			info.PrimeraEscritura.fecha = time(NULL);
			info.PrimeraEscritura.nEscritura = NUMESCRITURAS;
			info.PrimeraEscritura.nRegistro = 0;
			info.UltimaEscritura.fecha = 0;
			info.UltimaEscritura.nEscritura = 0;
			info.UltimaEscritura.nRegistro = 0;
			info.MenorPosicion.fecha = time(NULL);
			info.MenorPosicion.nEscritura = 0;
			info.MenorPosicion.nRegistro = REGMAX;
			info.MayorPosicion.fecha = time(NULL);
			info.MayorPosicion.nEscritura = 0;
			info.MayorPosicion.nRegistro = 0;
			

            // Obtenemos el camino hacia cada prueba.dat
            // dirAux = /simul_aaaammddhhmmss/entradas[i].nombre/prueba.dat
            char dAux[150];//direcotrio auxiliar
            strcpy(dAux, argv[2]);
            strcat(dAux, entradas[i].nombre);
            strcat(dAux, "/prueba.dat");

            // Preparamos el buffer siendo multiplo de BLOCKSIZE
            int nRegBuff = 256;
            struct REGISTRO writeBuff[nRegBuff];
            int offset = 0;
            
            int read = 1;
			while (info.nEscrituras < NUMESCRITURAS && read > 0)
            {
                read = mi_read(dAux,writeBuff,offset, sizeof(writeBuff));
                for (int j = 0; j < nRegBuff; j++)
                {
                    // Si la escritura es válida entonces
                    if (writeBuff[j].pid == info.pid)
                    {
                        // Si es la primera escritura validada entonces
                        if (writeBuff[j].nEscritura < info.PrimeraEscritura.nEscritura)
                        {
                            // Inicilizar los registros significativos con los datos de esa escritura
                            info.PrimeraEscritura.fecha = writeBuff[j].fecha;
                            info.PrimeraEscritura.nEscritura = writeBuff[j].nEscritura;
                            info.PrimeraEscritura.nRegistro = writeBuff[j].nRegistro;
                        }else{
                            double diff_t = difftime(info.UltimaEscritura.fecha,writeBuff[j].fecha);
                            if(diff_t < 0){
                                info.UltimaEscritura.fecha = writeBuff[j].fecha;
                                info.UltimaEscritura.nEscritura = writeBuff[j].nEscritura;
                                info.UltimaEscritura.nRegistro = writeBuff[j].nRegistro;
                            }else if(diff_t == 0){
                                if(writeBuff[j].nEscritura > info.UltimaEscritura.nEscritura){
                                    info.UltimaEscritura.fecha = writeBuff[j].fecha;
                                    info.UltimaEscritura.nEscritura = writeBuff[j].nEscritura;
                                    info.UltimaEscritura.nRegistro = writeBuff[j].nRegistro;
                                }
                            }
                        }
                        // Miramos la mayor y menor de las escrituras con nRegistro
                        if (writeBuff[j].nRegistro < info.MenorPosicion.nRegistro)
                        {
                            info.MenorPosicion.fecha = writeBuff[j].fecha;
                            info.MenorPosicion.nEscritura = writeBuff[j].nEscritura;
                            info.MenorPosicion.nRegistro = writeBuff[j].nRegistro;
                        }
                        if (writeBuff[j].nRegistro > info.MayorPosicion.nRegistro)
                        {
                            info.MayorPosicion.fecha = writeBuff[j].fecha;
                            info.MayorPosicion.nEscritura = writeBuff[j].nEscritura;
                            info.MayorPosicion.nRegistro = writeBuff[j].nRegistro;
                        }
                        info.nEscrituras++;
                    }
                }
                offset += read;
                memset(writeBuff, 0, sizeof(writeBuff));
            }

            //Añadimos la información del struct info al fichero
			memset(aux1,0,500);
			memset(aux2,0,500);
			sprintf(aux1, "PID: %d\n", info.pid);
            sprintf(aux2, "Numero escrituras: %d\n", info.nEscrituras);
            strcat(aux1, aux2);
            memset(aux2, 0, 100);
            sprintf(aux2, "Primera escritura\t%d\t%d\t%s", info.PrimeraEscritura.nEscritura, info.PrimeraEscritura.nRegistro, asctime(localtime(&info.PrimeraEscritura.fecha)));
            strcat(aux1, aux2);
            memset(aux2, 0, 100);
            sprintf(aux2, "Ultima escritura\t%d\t%d\t%s", info.UltimaEscritura.nEscritura, info.UltimaEscritura.nRegistro, asctime(localtime(&info.UltimaEscritura.fecha)));
            strcat(aux1, aux2);
            memset(aux2, 0, 100);
            sprintf(aux2, "Menor Posicion\t\t%d\t%d\t%s", info.MenorPosicion.nEscritura, info.MenorPosicion.nRegistro, asctime(localtime(&info.MenorPosicion.fecha)));
            strcat(aux1, aux2);
            memset(aux2, 0, 100);
            sprintf(aux2, "Mayor Posicion\t\t%d\t%d\t%s\n", info.MayorPosicion.nEscritura, info.MayorPosicion.nRegistro, asctime(localtime(&info.MayorPosicion.fecha)));
            strcat(aux1, aux2);

			infoPos += mi_write(fic,aux1,infoPos,strlen(aux1));
			fprintf(stderr,"[%d) %d escrituras validadas en %s]\n",i+1,info.nEscrituras,dAux);
		}
		bumount();
	} else {
		fprintf(stderr,"Uso: ./verificacion <disco> <directorio_simulacion>\n");
		return -1;
	}
}