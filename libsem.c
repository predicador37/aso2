//
// Created by Miguel Expósito Martín, 72056097H.
//
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "libsem.h"

Semaforo* semaforos = NULL;
int semid;


void a_sem(char* S, char tipo, int N) {

    key_t key;

    /* Comprobación del tipo de semáforo introducido */
    if (tipo != 'B' && tipo != 'G') {
        printf("[error] El tipo solo puede tomar los valores 'B' o 'G'.");
        exit(1);
    }

    /* Generación de la clave necesaria para obtener el conjunto de semáforos */
    if ((key = ftok(".", 'A')) == -1) {
        perror("[error] No se ha podido generar la clave para el conjunto de semáforos.");
        exit(1);
    }

    printf("[debug] La clave generada es: %d\n", key);

    /* Obtención del conjunto de semáforos */
    if ((semid = semget(key, N, 0666 | IPC_CREAT)) == -1) {
        perror("[error] No se ha podido crear el conjunto de semáforos");
        exit(1);
    }

    printf("[debug] El id del conjunto generado es: %d\n", semid);



    if (!semaforos) { // si es la primera vez que se llama, no hay semáforos
        semaforos = malloc(N * sizeof(Semaforo)); // se crea una estructura dinámicamente
        // y se asigna el primer semáforo
        semaforos[0].id = 0;
        semaforos[0].tipo = tipo;
        semaforos[0].nombre = malloc(sizeof(char) * (strlen(S) + 1));
        semaforos[0].nombre = S;
        printf("[debug] ID en la estructura: %d\n", semaforos[0].id);
        printf("[debug] TIPO en la estructura: %c\n", semaforos[0].tipo);
        printf("[debug] NOMBRE en la estructura: %s\n", semaforos[0].nombre);

    } else {  // si ya hay algún semáforo asignado:
        // se determina cuál es el siguiente sin asignar
        printf("[debug] Siguiente sin asignar: %d\n", siguiente_sin_asignar(N));
        int next = siguiente_sin_asignar(N);
        if (next > N) { // se captura la situación en la que ningún semáforo está libre
            perror("[error] No hay semáforos disponibles para asignar.");
            exit(1);
        }
        // y se asigna el siguiente semáforo
        semaforos[next].id = semaforos[next-1].id + 1;
        semaforos[next].tipo = tipo;
        semaforos[next].nombre = malloc(sizeof(char) * (strlen(S) + 1));
        semaforos[next].nombre = S;
        printf("[debug] ID en la estructura: %d\n", semaforos[next].id);
        printf("[debug] TIPO en la estructura: %c\n", semaforos[next].tipo);
        printf("[debug] NOMBRE en la estructura: %s\n", semaforos[next].nombre);

    }

}


void i_sem(char S[], int x) {

    // se busca el semáforo S en la estructura
    int sem = busca_semaforo(S);

    // se captura el caso en el que no se encuentre el semáforo
    if (sem == -1) {
        perror("[error] No se encuentra el semáforo.");
        exit(1);
    }
    // si el tipo de semáforo es binario y x no cumple no es 0 o 1...
    if (semaforos[sem].tipo == 'B' && condicion_binario(x) == 0) {
        perror("[error] El semáforo binario solo puede tomar valores 0 ó 1.");
        exit(1);
    }
    // si el tipo de semáforo es general y x no es un entero positivo mayor o gial que 0...
    if (semaforos[sem].tipo == 'G' && condicion_general(x) == 0) {
        perror("[error] El semáforo general solo puede tomar valores mayores o iguales que 0.");
        exit(1);
    }
    // se inicializa el semáforo con el valor especificado
    if (semctl(semid, semaforos[sem].id, SETVAL, x) == -1) {
        perror("[error] El semáforo no ha podido ser inicializado correctamente al valor especificado.");
        exit(1);
    }

}


void w_sem(char S[]) {

    // se busca el semáforo S en la estructura
    int sem = busca_semaforo(S);
    printf("[debug] w_sem: Semáforo encontrado: %d\n", sem);

    // se captura el caso en el que no se encuentre el semáforo
    if (sem == -1) {
        perror("[error] w_sem: No se encuentra el semáforo.");
        exit(1);
    }

    // se implementa la operación wait_sem decrementando el valor del semáforo en 1
    struct sembuf decrementar = {sem, -1, 0};
    if (semop(semid, &decrementar, 1) == -1) {
        perror("[error] w_sem: No es posible decrementar el semáforo.");
        exit(1);
    }

}


void s_sem(char S[]) {

    // se busca el semáforo S en la estructura
    int sem = busca_semaforo(S);

    // se captura el caso en el que no se encuentre el semáforo
    if (sem == -1) {
        perror("[error] s_sem: No se encuentra el semáforo.");
        exit(1);
    }

    int semValue = semctl(semid, sem, GETVAL, 0);

    //si el semáforo es binario y ya se encuentra a 1, volver
    if (semaforos[sem].tipo == 'B' && semValue == 1) {
        return;
    }

    // se implementa la operación signal_sem incrementando el valor del semáforo en 1
    struct sembuf incrementar = {sem, 1, 0};
    if (semop(semid, &incrementar, 1) == -1) {
        perror("[error] s_sem: No es posible incrementar el semáforo.");
        exit(1);
    }
    return;
}

void z_sem(){

    // se destruye el conjunto de semáforos
    if (semctl(semid, 0, IPC_RMID) == -1 ) {
        perror("[error] z_sem: No es posible destruir el conjunto de semáforos.");
        exit(1);
    }
    int i=0;

    // se libera la memoria reservada para los nombres de los semáforos
    while (semaforos[i].nombre != NULL){
        free(semaforos[i].nombre);
        i++;
    }

    // se libera la memoria reservada para la estructura que almacena los semáforos
    free(semaforos);
}

int obtener_valor_semaforo(int sem) {
    int semValue = semctl(semid, sem, GETVAL, 0);
    printf("[debug] valor obtenido: %d\n", semValue);
}

/*
 * siguiente_sin_asignar
 *
 * Función auxiliar que determina cuál es el siguiente semáforo sin asignar.
 * Para ello, devuelve el índice del primer semáforo sin nombre asignado en la estructura.
 *
 */
int siguiente_sin_asignar() {
    int n = 0;
    while (semaforos[n].nombre != NULL){
        n++;
    }
    return n;
}


/*
 * busca_semaforo
 *
 * Parámetros de entrada:
 * char S[]: nombre del semáforo
 *
 * Descripción:
 *
 * Esta función comprueba cada semáforo de la estructura y, si su nombre no es nulo, lo compara con el nombre S.
 * Si lo encuentra, lo devuelve; en otro caso, devuelve -1.
 *
 */
int busca_semaforo(char S[]) {
    int n = 0;
    while (semaforos[n].nombre != NULL){
        if (semaforos[n].nombre == S) {
            return n;
        }
        n++;
    }
    return -1;
}

/*
 * condicion_binario
 *
 * Parámetros de entrada:
 * int x: valor a fijar en el semáforo
 *
 * Descripción:
 *
 * Esta función comprueba si el valor a fijar cumple con las condiciones de un valor para semáforo binario.
 *
 */
int condicion_binario(int x) {
    if (x == 0 || x == 1) {
        return 1;
    }
    else {
        return 0;
    }
}

/*
 * condicion_general
 *
 * Parámetros de entrada:
 * int x: valor a fijar en el semáforo
 *
 * Descripción:
 *
 * Esta función comprueba si el valor a fijar cumple con las condiciones de un valor para semáforo general.
 *
 */
int condicion_general(int x) {
    if (x >= 0) {
        return 1;
    }
    else {
        return 0;
    }
}
