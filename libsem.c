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

   /* *//* Inicialización de los semáforos a estado disponible *//*
    arg = 1;
    for(int n=0; n<N; n++) {
        if (semctl(semid, n, SETVAL, arg) == -1) {
            perror("semctl");
            exit(1);
        }
    }*/
    // si no hay ningún semáforo asignado,
    // inicializar el array de structs con malloc
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
    printf("[debug] Semáforo encontrado: %d\n", sem);
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
    printf("[debug] Valor leído del semáforo: %d\n", semValue);


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
