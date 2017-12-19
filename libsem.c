//
// Created by predicador on 9/12/17.
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

    if (!semaforos) {
        semaforos = malloc(N * sizeof(Semaforo));
        // y asignar el primer semáforo
        semaforos[0].id = 0;
        semaforos[0].tipo = tipo;
        semaforos[0].nombre = malloc(sizeof(char) * (strlen(S) + 1));
        semaforos[0].nombre = S;
        printf("[debug] ID en la estructura: %d\n", semaforos[0].id);
        printf("[debug] TIPO en la estructura: %c\n", semaforos[0].tipo);
        printf("[debug] NOMBRE en la estructura: %s\n", semaforos[0].nombre);

    } else {  // si ya hay algún semáforo asignado:
        // cuál es el siguiente sin asignar
        printf("[debug] Siguiente sin asignar: %d\n", siguiente_sin_asignar(N));
        int next = siguiente_sin_asignar(N);
        if (next > N) {
            perror("[error] No hay semáforos disponibles para asignar.");
            exit(1);
        }
        semaforos[next].id = semaforos[next-1].id + 1;
        semaforos[next].tipo = tipo;
        semaforos[next].nombre = malloc(sizeof(char) * (strlen(S) + 1));
        semaforos[next].nombre = S;
        printf("[debug] ID en la estructura: %d\n", semaforos[next].id);
        printf("[debug] TIPO en la estructura: %c\n", semaforos[next].tipo);
        printf("[debug] NOMBRE en la estructura: %s\n", semaforos[next].nombre);

    }





    // asignar o pasar al siguiente si ya estaba asignado o no estaba disponible



}

void i_sem(char S[], int x) {
    int sem = busca_semaforo(S);
    printf("[debug] i_sem: Semáforo encontrado: %d\n", sem);
    if (sem == -1) {
        perror("[error] No se encuentra el semáforo.");
        exit(1);
    }
    if (semaforos[sem].tipo == 'B' && condicion_binario(x) == 0) {
        perror("[error] El semáforo binario solo puede tomar valores 0 ó 1.");
        exit(1);
    }
    if (semaforos[sem].tipo == 'G' && condicion_general(x) == 0) {
        perror("[error] El semáforo general solo puede tomar valores mayores o iguales que 0.");
        exit(1);
    }
    if (semctl(semid, semaforos[sem].id, SETVAL, x) == -1) {
        perror("[error] El semáforo no ha podido ser inicializado correctamente al valor especificado.");
        exit(1);
    }
    int semValue = semctl(semid, sem, GETVAL, 0);
    printf("[debug] i_sem: Valor leído del semáforo %s: %d\n", S, semValue);


}

void w_sem(char S[]) {
    printf("INICIO WSEM: semaforo %s\n", S);
    int sem = busca_semaforo(S);
    printf("[debug] w_sem: Semáforo encontrado: %d\n", sem);
    if (sem == -1) {
        perror("[error] w_sem: No se encuentra el semáforo.");
        exit(1);
    }

    struct sembuf decrementar = {sem, -1, 0};
    if (semop(semid, &decrementar, 1) == -1) {
        perror("[error] w_sem: No es posible decrementar el semáforo.");
        exit(1);
    }
    int semValue = semctl(semid, sem, GETVAL, 0);
    printf("[debug] w_sem: valor leído del semáforo %s: %d\n", S, semValue);
    printf("END OF WSEM\n");
    return;

}

void s_sem(char S[]) {
    int sem = busca_semaforo(S);
    printf("[debug] s_sem: Semáforo encontrado: %d\n", sem);
    if (sem == -1) {
        perror("[error] s_sem: No se encuentra el semáforo.");
        exit(1);
    }

    int semValue = semctl(semid, sem, GETVAL, 0);


    if (semaforos[sem].tipo == 'B' && semValue == 1) {
        return;
    }

    struct sembuf incrementar = {sem, 1, 0};

    if (semop(semid, &incrementar, 1) == -1) {
        perror("[error] s_sem: No es posible incrementar el semáforo.");
        exit(1);
    }

    semValue = semctl(semid, sem, GETVAL, 0);
    printf("[debug] s_sem: valor leído del semáforo %s: %d\n", S, semValue);
    return;
}

void z_sem(){

    if (semctl(semid, 0, IPC_RMID) == -1 ) {
        perror("[error] z_sem: No es posible destruir el conjunto de semáforos.");
        exit(1);
    }
    int i=0;
    while (semaforos[i].nombre != NULL){
        free(semaforos[i].nombre);
        i++;
    }
    free(semaforos);
}

int obtener_valor_semaforo(int sem) {
    int semValue = semctl(semid, sem, GETVAL, 0);
    printf("[debug] valor obtenido: %d\n", semValue);
}

int siguiente_sin_asignar() {
    int n = 0;
    while (semaforos[n].nombre != NULL){
        n++;
    }
    return n;
}

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

int condicion_binario(int x) {
    if (x == 0 || x == 1) {
        return 1;
    }
    else {
        return 0;
    }
}

int condicion_general(int x) {
    if (x >= 0) {
        return 1;
    }
    else {
        return 0;
    }
}
