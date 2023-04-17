/**
 * Equipo: Alea iacta est
 * Alzamora Llabata, Pere
 * Arenas Afán de Rivera, Francisco
 * Reyes Gómez, Andrea Valentina
 */

#include "ficheros.h"

int main(int argc, char **argv){
    struct STAT p_stat;
    struct tm *ts;
	char atime[80];
	char mtime[80];
	char ctime[80];

    //Validación de sintaxis
    if (argc != 4){
        fprintf(stderr,"Sintaxis correcta: truncar <nombre_dispositivo> <ninodo> <bytes>\n");
        return -1;
    }

    if(bmount(argv[1])<0) return -1; //Montamos dispositivo virtual

    if(atoi(argv[3]) == 0){ // Cantidad de bytes igual a 0
        liberar_inodo(atoi(argv[2]));
    }else{
        mi_truncar_f(atoi(argv[2]),atoi(argv[3]));
    }

    mi_stat_f(atoi(argv[2]),&p_stat);
    
    //Desmontamos el dispositivo virtual
    
    fprintf(stderr,"\nDATOS INODO %d\n",atoi(argv[2]));
    fprintf(stderr,"tipo: %c\n",p_stat.tipo);
    fprintf(stderr,"permisos: %d\n",p_stat.permisos);
    ts = localtime(&p_stat.atime);
	strftime(atime, sizeof(atime), "%a %Y-%m-%d %H:%M:%S", ts);
	ts = localtime(&p_stat.mtime);
	strftime(mtime, sizeof(mtime), "%a %Y-%m-%d %H:%M:%S", ts);
	ts = localtime(&p_stat.ctime);
	strftime(ctime, sizeof(ctime), "%a %Y-%m-%d %H:%M:%S", ts);
	printf("atime: %s \nmtime: %s \nctime: %s\n",atime,mtime,ctime);
    fprintf(stderr,"nlinks: %d\n",p_stat.nlinks);
    fprintf(stderr,"TamEnBytesLog: %d\n", p_stat.tamEnBytesLog);
    fprintf(stderr,"numBloquesOcupados: %d\n",p_stat.numBloquesOcupados);

    bumount();
}