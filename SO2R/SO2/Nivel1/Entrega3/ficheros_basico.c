/**
 * Equipo: Alea iacta est
 * Alzamora Llabata, Pere
 * Arenas Afán de Rivera, Francisco
 * Reyes Gómez, Andrea Valentina
 */

#include "ficheros_basico.h"

int bwriter = 0, br = 0; // Bytes Written

/*
 * Función: tamMB()
 * Calcula el tamaño en bloques necesario para el mapa de bits
 * Input        nbloques: número de bloques
 * Output       numero de bloques necesarios para el mapa de bits
 *             -1: error de ejecución
 */
int tamMB(unsigned int nbloques)
{
    if (((nbloques / 8) % BLOCKSIZE) == 0)
        return (nbloques / 8) / BLOCKSIZE;
    else
        return ((nbloques / 8) / BLOCKSIZE) + 1;
}

/*
 * Función: tamAI()
 * Calcula el tamaño en bloques del array de inodos
 * Input        ninodos: número de inodos
 * Output       tamaño del array de inodos
 *             -1: error de ejecución
 */
int tamAI(unsigned int ninodos)
{
    if (((ninodos * INODOSIZE) % BLOCKSIZE) == 0)
    {
        return ((ninodos * INODOSIZE) / BLOCKSIZE);
    }
    else
    {
        return ((ninodos * INODOSIZE) / BLOCKSIZE) + 1;
    }
}

/*
 * Función: initSB()
 * Inicializa los datos del superbloque
 * Input        nbloques: número de bloques
 *              ninodos: número de inodos
 * Output       0: ejecución correcta
 *             -1: error de ejecución
 */
int initSB(unsigned int nbloques, unsigned int ninodos)
{
    struct superbloque SB;
    SB.posPrimerBloqueMB = posSB + tamSB;
    SB.posUltimoBloqueMB = SB.posPrimerBloqueMB + tamMB(nbloques) - 1;
    SB.posPrimerBloqueAI = SB.posUltimoBloqueMB + 1;
    SB.posUltimoBloqueAI = SB.posPrimerBloqueAI + tamAI(ninodos) - 1;
    SB.posPrimerBloqueDatos = SB.posUltimoBloqueAI + 1;
    SB.posUltimoBloqueDatos = nbloques - 1;
    SB.posInodoRaiz = 0;
    SB.posPrimerInodoLibre = 0; // Despues debe apuntar a la cabeza de la lista de inodos
                                // libres (mediante las llamadas a las funciones reservar_inodo() y liberar_inodo())

    SB.cantBloquesLibres = nbloques; // Esta cantidad cambia cuando se reserva o se libera un bloque
    SB.cantInodosLibres = ninodos;   // Esta cantidad cambia cuando se reserva o se libera un bloque
    SB.totBloques = nbloques;
    SB.totInodos = ninodos; // Este valor está indicando en mi_mkfs.c

    bwriter = bwrite(posSB, &SB);
    if (bwriter < 0)
        return -1;

    return 0;
}

/*
 * Función: initMB()
 * Inicializa el mapa de bits
 * Input        void
 * Output       0: ejecución correcta
 *             -1: error de ejecución
 */
int initMB()
{
    struct superbloque SB;
    unsigned char bufferMB[BLOCKSIZE];

    memset(bufferMB, 0, BLOCKSIZE);
    br = bread(posSB, &SB);

    // Se ponen a 1 en el MB los bits que corresponden a los bloques que ocupa
    // el superbloque, el propio MB, y el array de inodos
    for (int i = 0; i <= SB.posUltimoBloqueAI; i++)
    {
        if (escribir_bit(i, 1) == -1)
            return -1;
    }
    SB.cantBloquesLibres = SB.cantBloquesLibres - (SB.posUltimoBloqueAI + 1);
    bwriter = bwrite(posSB, &SB);
    if (bwriter < 0)
        return -1;
    return 0;
}

/*
 * Función: initAI()
 * Inicializa la lista de inodos libres
 * Input        void
 * Output       0: ejecución correcta
 *             -1: error de ejecución
 */
int initAI()
{
    struct inodo inodos[BLOCKSIZE / INODOSIZE];
    struct superbloque SB;
    bread(0, &SB);
    int contInodos = SB.posPrimerInodoLibre + 1;
    int i, j;
    for (i = SB.posPrimerBloqueAI; i <= SB.posUltimoBloqueAI; i++)
    {
        for (j = 0; j < (BLOCKSIZE / INODOSIZE); j++)
        {
            inodos[j].tipo = 'l';
            if (contInodos < SB.totInodos)
            {
                inodos[j].punterosDirectos[0] = contInodos;
                contInodos++;
            }
            else
            {
                inodos[j].punterosDirectos[0] = UINT_MAX;
                break;
            }
        }
        if (bwrite(i, inodos) < 0)
        {
            fprintf(stderr, "Error en InitAI\n");
            return -1;
        }
    }
    return 0;
}

//------------- NIVEL 3 -------------------

/*
 * Función: escribir_bit()
 * Escribe el valor indicado por el parametro bit en un determinado bit del MB
 * Input        nbloque: indice del bloque que queremos cambiar en el MB
 *              bit: valor del bit que queremos asignar al bit del MB
 * Output       0: ejecución correcta
 *             -1: error de ejecución
 */
int escribir_bit(unsigned int nbloque, unsigned int bit)
{
    int posbyte, posbit, nbloqueMB, nbloqueabs;
    unsigned char *bufferMB = malloc(BLOCKSIZE);
    unsigned char mascara = 128; //10000000

    struct superbloque SB;
    bread(posSB, &SB);

    posbyte = nbloque / 8;
    posbit = nbloque % 8;
    nbloqueMB = posbyte / BLOCKSIZE;               //Determinamos el bloque en la memoria virtual
    nbloqueabs = SB.posPrimerBloqueMB + nbloqueMB; //Sumamos el primer bloque para obtener la direccion absoluta

    if (bread(nbloqueabs, bufferMB) < 0)
    {
        return -1;
    }

    posbyte = posbyte % BLOCKSIZE;

    mascara >>= posbit; //Desplazamiento de bits a la derecha

    if (bit == 1)
    {
        bufferMB[posbyte] |= mascara; // OR
    }
    else
    {
        bufferMB[posbyte] &= ~mascara; // AND y NOT
    }

    if (bwrite(nbloqueabs, bufferMB) < 0)
    {
        return -1;
    }
    //free(bufferMB);										//Liberamos el buffer
    return 0;
}

/*
 * Función: leer_bit()
 * lee el valor del bit del MB indicado
 * Input        nbloque: indice del bloque que queremos leer
 *              
 * Output       0/1: valor del big leido
 *             -1: error de ejecución
 */
char leer_bit(unsigned int nbloque)
{
    int posbyte, posbit, nbloqueMB, nbloqueabs;
    unsigned char *bufferMB = malloc(BLOCKSIZE);
    ;
    unsigned char mascara = 128; //10000000
    struct superbloque SB;
    bread(posSB, &SB);

    posbyte = nbloque / 8;
    posbit = nbloque % 8;
    nbloqueMB = posbyte / BLOCKSIZE;               //Determinamos el bloque en la memoria virtual
    nbloqueabs = SB.posPrimerBloqueMB + nbloqueMB; //Sumamos el primer bloque para obtener la direccion absoluta

    if (bread(nbloqueabs, bufferMB) < 0)
    {
        return -1;
    }

    posbyte = posbyte % BLOCKSIZE;
    mascara >>= posbit;           //Desplazamiento de bits a la derecha
    mascara &= bufferMB[posbyte]; //And para bits
    mascara >>= (7 - posbit);     //desplazamiento de bits a la derecha
                                  //free(bufferMB);

    return mascara;
}

/*
 * Función: reservar_bloque()
 * Encuentra el primer bloque libre, consultando el MB, lo ocupa y 
 * devuelve su posición.
 * Input        void
 * Output       nbloque: numero del bloque que se ha reservado
 */
int reservar_bloque()
{
    struct superbloque SB;
    bread(posSB, &SB);
    unsigned char mascara = 128; // 1000000
    int i, posbyte, nbloque, posBloqueMB;
    unsigned char *bufferAux = malloc(BLOCKSIZE);
    unsigned char *bufferMB = malloc(BLOCKSIZE);

    memset(bufferAux, 255, BLOCKSIZE);

    if (SB.cantBloquesLibres > 0)
    {
        posBloqueMB = SB.posPrimerBloqueMB;
        while (posBloqueMB <= SB.posUltimoBloqueMB)
        {
            if (bread(posBloqueMB, bufferMB) < 0)
            {
                return -1;
            }

            // Si comp<0 se ha encontrado un bloque vacio
            if (memcmp(bufferMB, bufferAux, BLOCKSIZE) != 0)
            {
                break;
            }
            posBloqueMB++;
        }

        for (i = 0; i < BLOCKSIZE; i++)
        {
            if (bufferMB[i] != 255)
            {
                posbyte = i;
                break;
            }
        }

        int posbit = 0;
        // Encontrar el primer bit a 0 en el byte bufferMB[posbyte]
        while (bufferMB[posbyte] & mascara)
        {                            // Operador AND para bits
            bufferMB[posbyte] <<= 1; // Desplazamiento de bits a la izquierda
            posbit++;
        }

        nbloque = ((posBloqueMB - SB.posPrimerBloqueMB) * BLOCKSIZE + posbyte) * 8 + posbit;

        if (escribir_bit(nbloque, 1) < 0)
            return -1;

        SB.cantBloquesLibres = SB.cantBloquesLibres - 1;
        bwriter = bwrite(posSB, &SB); // Se guardan los nuevos valores del SB
        if (bwriter < 0)
            return -1;
        memset(bufferAux, 0, BLOCKSIZE);
        bwriter = bwrite(nbloque, bufferAux);
        // Se limpia el bloque por si habia basura
        if (bwriter < 0)
            return -1;

        return nbloque;
    }
    return -1;
}

/*
 * Función: liberar_bloque()
 * Libera un bloque determinado (con la ayuda de la función escribir_bit())
 * Input        nbloque: número del bloque a liberar
 * Output       nbloque: numero del bloque liberado
 */
int liberar_bloque(unsigned int nbloque)
{
    struct superbloque SB;
    if (bread(posSB, &SB) < 0)
        return -1;

    bwriter = escribir_bit(nbloque, 0);
    if (bwriter < 0)
        return -1;

    SB.cantBloquesLibres = SB.cantBloquesLibres + 1;
    bwriter = bwrite(posSB, &SB); // Se salva el SB
    if (bwriter < 0)
        return -1;

    return nbloque; // Numero de bloque liberado
}

/*
 * Función: escribir_inodo()
 * Escribe el contenido de una variable de tipo struct inodo en un determinado
 * inodo del array de inodos, inodos.
 * Input        ninodo: número de inodo en el array de inodos
 *              inodo: contenido a escribir
 * Output       -1: error de ejecución
 */
int escribir_inodo(unsigned int ninodo, struct inodo inodo)
{
    struct superbloque SB;
    struct inodo inodos[BLOCKSIZE / INODOSIZE];
    if (bread(posSB, &SB) < 0)
        return -1;
    int nbloqueabs = (ninodo * INODOSIZE) / BLOCKSIZE + SB.posPrimerBloqueAI;
    if (bread(nbloqueabs, inodos) < 0)
        return -1;
    inodos[ninodo % (BLOCKSIZE / INODOSIZE)] = inodo;
    if (bwrite(nbloqueabs, inodos) < 0)
    {
        return -1;
    }
    return 0;
}

/*
 * Función: leer_inodo()
 * Lee un determinado inodo del array de inodos para volcarlo en una variable
 * de tipo struct inodo pasada por referencia.
 * inodo del array de inodos, inodos.
 * Input        ninodo: número de inodo en el array de inodos
 *              inodo: contenido a leer
 * Output        0: ejecución correcta
 *              -1: error de ejecución
 */
int leer_inodo(unsigned int ninodo, struct inodo *inodo)
{
    struct superbloque SB;
    if (bread(posSB, &SB) < 0)
    {
        return -1;
    }
    else
    {
        struct inodo inodos[BLOCKSIZE / INODOSIZE];
        int nbloqueabs = (ninodo * INODOSIZE) / BLOCKSIZE + SB.posPrimerBloqueAI;
        if (bread(nbloqueabs, inodos) < 0)
            return -1;
        *inodo = inodos[ninodo % (BLOCKSIZE / INODOSIZE)];
        return 0;
    }
}

/*
 * Función: reservar_inodo()
 * Encuentra el primer inodo libre (dato almacenado en el superbloque), 
 * lo reserva (con la ayuda de la función escribir_inodo()), devuelve su número 
 * y actualiza la lista enlazada de inodos libres.
 * Input        tipo: tipo del inodo al que apunta el superbloque
 *              permisos: permisos del inodo al que apunta el superbloque
 * Output       posInodoReservado: posición del inodo reservado
 */
int reservar_inodo(unsigned char tipo, unsigned char permisos)
{
    struct inodo inodo;
    struct superbloque SB;
    if (bread(posSB, &SB) < 0)
    {
        fprintf(stderr, "Error, no se pudo leer el superbloque");
        return -1;
    }
    else
    {
        if (SB.cantInodosLibres <= 0)
        {
            fprintf(stderr, "Error, no quedan inodos libres");
            return -1;
        }

        int posInodoReservado = SB.posPrimerInodoLibre;

        inodo.tipo = tipo;
        inodo.permisos = permisos;
        inodo.nlinks = 1;
        inodo.tamEnBytesLog = 0;
        inodo.atime = time(NULL);
        inodo.mtime = time(NULL);
        inodo.ctime = time(NULL);
        inodo.numBloquesOcupados = 0;
        int i, j;
        for (i = 0; i < 12; i++)
        {
            for (j = 0; j < 3; j++)
            {
                inodo.punterosIndirectos[j] = 0;
            }
            inodo.punterosDirectos[i] = 0;
        }

        if (escribir_inodo(posInodoReservado, inodo) < 0)
        {
            //fprintf(stderr,"Error, no se pudo escribir inodo");
            return -1;
        }
        SB.cantInodosLibres = SB.cantInodosLibres - 1;
        SB.posPrimerInodoLibre++;
        bwrite(posSB, &SB);
        return posInodoReservado;
    }
}

//------------------------------ NIVEL 4 --------------------------------

/*
 * Función: obtener_nRangoBL()
 * Obtiene el rango de punteros en el que se sitúa el bloque lógico
 * y la dirección almacenada en el puntero correspondiente del inodo
 * Input        inodo: contenido a traducir
 *              nblogico: número del bloque lógico
 *              ptr: puntero que apunta donde lo hace el puntero del inodo
 * Output       Bloque lógico: 
 *                  0: D, 1: I0, 2: I1, 3: I2
 *              -1: error de ejecución
 */
int obtener_nRangoBL(struct inodo *inodo, unsigned int nblogico, unsigned int *ptr)
{
    if (nblogico < DIRECTOS)
    {
        *ptr = inodo->punterosDirectos[nblogico];
        return 0;
    }
    else if (nblogico < INDIRECTOS0)
    {
        *ptr = inodo->punterosIndirectos[0];
        return 1;
    }
    else if (nblogico < INDIRECTOS1)
    {
        *ptr = inodo->punterosIndirectos[1];
        return 2;
    }
    else if (nblogico < INDIRECTOS2)
    {
        *ptr = inodo->punterosIndirectos[2];
        return 3;
    }
    else
    {
        *ptr = 0;
        //fprintf(stderr, "Bloque lógico fuera de rango\n");
        return -1;
    }
}

/*
 * Función: obtener_indice()
 * Obtiene los índices de los bloques de punteros
 * Input        nivel_punteros: nivel en el que se encuentra el puntero
 *              nblogico: número del bloque lógico
 * Output       ind: Índice del bloque de punteros
 */
int obtener_indice(int nblogico, int nivel_punteros)
{
    if (nblogico < DIRECTOS)
    {
        return nblogico;
    }
    else if (nblogico < INDIRECTOS0)
    {
        return nblogico - DIRECTOS;
    }
    else if (nblogico < INDIRECTOS1)
    {
        if (nivel_punteros == 2)
        {
            return (nblogico - INDIRECTOS0) / NPUNTEROS;
        }
        else if (nivel_punteros == 1)
        {
            return (nblogico - INDIRECTOS0) % NPUNTEROS;
        }
    }
    else if (nblogico < INDIRECTOS2)
    {
        if (nivel_punteros == 3)
        {
            return (nblogico - INDIRECTOS1) / (NPUNTEROS * NPUNTEROS);
        }
        else if (nivel_punteros == 2)
        {
            return ((nblogico - INDIRECTOS1) % (NPUNTEROS * NPUNTEROS)) / NPUNTEROS;
        }
        else if (nivel_punteros == 1)
        {
            return ((nblogico - INDIRECTOS1)) % (NPUNTEROS * NPUNTEROS) % NPUNTEROS;
        }
    }
    return -1;
}

/*
 * Función: traducir_bloque_inodo()
 * Obtiene el nº  de bloque físico correspondiente a un bloque lógico 
 * determinado del inodo indicado
 * Input        ninodo: número de inodo en el array de inodos
 *              nblogico: número del bloque lógico
 *              reservar: indica si se desea reservar o solo consultar
 * Output       prt: puntero
 *               -1: error de ejecución
 */
int traducir_bloque_inodo(unsigned int ninodo, unsigned int nblogico, char reservar)
{
    struct inodo inodo;
    unsigned int ptr, ptr_ant;
    int salvar_inodo, indice, nivel_punteros, nRangoBL;
    int buffer[NPUNTEROS];

    if (leer_inodo(ninodo, &inodo) < 0)
        return -1;
    ptr = 0;
    ptr_ant = 0;
    salvar_inodo = 0;
    nRangoBL = obtener_nRangoBL(&inodo, nblogico, &ptr);
    nivel_punteros = nRangoBL;
    while (nivel_punteros > 0)
    { // Iterar para cada nivel de indirectos
        if (ptr == 0)
        {
            if (reservar == 0)
            {
                return -1; // Error: lectura de bloque inexistente
            }
            else
            {
                salvar_inodo = 1;
                ptr = reservar_bloque(); // De punteros
                inodo.numBloquesOcupados++;
                inodo.ctime = time(NULL); // Fecha actual
                if (nivel_punteros == nRangoBL)
                {
                    inodo.punterosIndirectos[nRangoBL - 1] = ptr;
                    //fprintf(stderr, "[traducir_bloque_inodo() → inodo.punterosIndirectos[%d] = %d (reservado BF %d para punteros_nivel%d)]\n",
                            //nRangoBL - 1, inodo.punterosIndirectos[nRangoBL - 1], inodo.punterosIndirectos[nRangoBL - 1], nRangoBL);
                }
                else
                {
                    buffer[indice] = ptr;
                    if (bwrite(ptr_ant, buffer) == -1)
                        return -1;
                    //fprintf(stderr, "[traducir_bloque_inodo() → inodo.punteros_nivel%d[%d] = %d (reservado BF %d para punteros_nivel%d)]\n",
                            //nivel_punteros + 1, indice, buffer[indice], buffer[indice], nivel_punteros);
                }
            }
        }
        bread(ptr, buffer);
        indice = obtener_indice(nblogico, nivel_punteros);
        ptr_ant = ptr;        // Se guarda el puntero
        ptr = buffer[indice]; // Desplazar al siguiente nivel
        nivel_punteros--;
    }
    if (ptr == 0)
    {
        if (reservar == 0)
        {
            return -1;
        }
        else
        {
            salvar_inodo = 1;
            ptr = reservar_bloque(); // De datos
            inodo.numBloquesOcupados++;
            inodo.ctime = time(NULL);
            if (nRangoBL == 0)
            {
                inodo.punterosDirectos[nblogico] = ptr;
                //fprintf(stderr, "[traducir_bloque_inodo() → inodo.punterosDirectos[%d] = %d (reservado BF %d para BL %d)]\n",
                       // nblogico, inodo.punterosDirectos[nblogico], inodo.punterosDirectos[nblogico], nblogico);
            }
            else
            {
                buffer[indice] = ptr;
                bwrite(ptr_ant, buffer);
                //fprintf(stderr, "[traducir_bloque_inodo() → inodo.punteros_nivel%d[%d] = %d (reservado BF %d para BL %d)]\n",
                        //nivel_punteros + 1, indice, buffer[indice], buffer[indice], nblogico);
            }
        }
    }
    if (salvar_inodo == 1)
    {
        escribir_inodo(ninodo, inodo); // Solo si se ha actualizado
    }
    return ptr;
}

//------------------------------ NIVEL 6 --------------------------------
/*
 * Función: liberar_inodo()
 * Libera un inodo
 * Input        ninodo: número de inodo en el array de inodos
 * Output       ninodo: número del inodo liberado
 *               -1: error de ejecución
 */
int liberar_inodo(unsigned int ninodo)
{
    struct superbloque SB;
    struct inodo my_inodo;
    int bloques_liberados;
    bread(posSB, &SB);
    leer_inodo(ninodo, &my_inodo);

    bloques_liberados = liberar_bloques_inodo(0, &my_inodo);

    my_inodo.numBloquesOcupados = my_inodo.numBloquesOcupados - bloques_liberados; //Tiene que dar 0

    //Comprobamos que coincidan los bloques liberados con el tamaño en bytes
    my_inodo.tipo = 'l'; //Poner el inodo tipo libre
    my_inodo.tamEnBytesLog = 0;
    //Actualizar lista enlazada de inodos libres:
    if (bread(posSB, &SB) < 0)
        return -1; //Se vuelve a leer porque liberar_bloques_inodo() lo modifica

    SB.posPrimerInodoLibre = ninodo;
    my_inodo.punterosDirectos[0] = SB.posPrimerInodoLibre;

    SB.cantInodosLibres++;
    if (escribir_inodo(ninodo, my_inodo) < 0)
        return -1;
    if (bwrite(posSB, &SB) < 0)
        return -1;

    return (ninodo);
}

/*
 * Función: liberar_bloques_ inodo()
 * Libera los bloques de datos e índices iterando desde el primer bloque
 * lógico a liberar hasta el último
 * Input         primerBL: bloque lógico a partir del cual se liberará
 *                  inodo: puntero al inodo
 * Output       liberados: número de inodos liberados
 *                     -1: error de ejecución
 */
int liberar_bloques_inodo(unsigned int primerBL, struct inodo *inodo)
{
    unsigned int nivel_punteros, indice, ptr, nBL, ultimoBL;
    int nRangoBL;
    unsigned int bloques_punteros[3][NPUNTEROS]; //array de bloques de punteros
    unsigned char bufAux_punteros[BLOCKSIZE];
    int ptr_nivel[3]; //punteros a bloques de punteros de cada nivel
    int indices[3];   //indices de cada nivel
    int liberados;    // nº de bloques liberados

    liberados = 0;

    if (inodo->tamEnBytesLog == 0)
        return 0; //El fichero está vacío
    //Obtenemos el último bloque lógico del inodo
    if (inodo->tamEnBytesLog % BLOCKSIZE == 0)
    {
        ultimoBL = inodo->tamEnBytesLog / BLOCKSIZE - 1;
    }
    else
    {
        ultimoBL = inodo->tamEnBytesLog / BLOCKSIZE;
    }

    memset(bufAux_punteros, 0, BLOCKSIZE);
    ptr = 0;
    //fprintf(stderr, "[liberar_bloques_inodo()→ primer BL: %d, último BL: %d]\n", primerBL, ultimoBL);
    for (nBL = primerBL; nBL <= ultimoBL; nBL++)
    {                                                  //Revisar las condiciones de este for
        nRangoBL = obtener_nRangoBL(inodo, nBL, &ptr); //0:D, 1:I0, 2:I1, 3:I2
        if (nRangoBL < 0)
        {
            fprintf(stderr, "Error");
            return -1;
        }
        nivel_punteros = nRangoBL; //el nivel_punteros +alto cuelga del inodo

        while (ptr > 0 && nivel_punteros > 0)
        { //cuelgan bloques de punteros
            indice = obtener_indice(nBL, nivel_punteros);

            if (indice == 0 || nBL == primerBL)
            {
                //solo leemos del dispositivo si no está ya cargado previamente en un buffer
                bread(ptr, bloques_punteros[nivel_punteros - 1]);
            }
            ptr_nivel[nivel_punteros - 1] = ptr;
            indices[nivel_punteros - 1] = indice;
            ptr = bloques_punteros[nivel_punteros - 1][indice];
            nivel_punteros--;
        }

        if (ptr > 0)
        {
            liberar_bloque(ptr);
            liberados++;
            //fprintf(stderr, "[liberar_bloques_inodo()→ liberado BF %d de datos para BL: %d]\n", ptr, nBL);
            if (nRangoBL == 0)
            {
                inodo->punterosDirectos[nBL] = 0;
            }
            else
            {
                nivel_punteros = 1;
                while (nivel_punteros <= nRangoBL)
                {
                    indice = indices[nivel_punteros - 1];
                    bloques_punteros[nivel_punteros - 1][indice] = 0;
                    ptr = ptr_nivel[nivel_punteros - 1];
                    if (memcmp(bloques_punteros[nivel_punteros - 1], bufAux_punteros, BLOCKSIZE) == 0)
                    {
                        //No cuelgan más bloques ocupados, hay que liberar el bloque de punteros
                        liberar_bloque(ptr);
                        liberados++;
                        //fprintf(stderr, "[liberar_bloques_inodo()→ liberado BF %d de punteros_nivel%d correspondiente al BL: %d]\n", ptr, nivel_punteros, nBL);
                        if (nivel_punteros == nRangoBL)
                        {
                            inodo->punterosDirectos[nRangoBL - 1] = 0;
                        }
                        nivel_punteros++;
                    }
                    else
                    { //escribimos en el dispositivo el bloque de punteros modificado
                        bwrite(ptr, bloques_punteros[nivel_punteros - 1]);
                        // hemos de salir del bucle ya que no será necesario liberar los bloques de niveles
                        // superiores de los que cuelga
                        nivel_punteros = nRangoBL + 1;
                    }
                }
            }
        }
    }
    // fprintf(stderr, "liberados: %d\n", liberados);
    return liberados;
}
