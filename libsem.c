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


void a_sem(char* S, char tipo, int N) {



    key_t key;
    int semid, arg;


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

    /* Inicialización de los semáforos a estado disponible */
    arg = 1;
    for(int n=0; n<N; n++) {
        if (semctl(semid, n, SETVAL, arg) == -1) {
            perror("semctl");
            exit(1);
        }
    }
    // si no hay ningún semáforo asignado,
    // asignar memoria al array de structs con malloc
    if (!semaforos) {
        semaforos = malloc(N * sizeof(Semaforo));
        semaforos[0].id = 0;
        semaforos[0].tipo = tipo;
        semaforos[0].nombre = malloc(sizeof(char) * (strlen(S) + 1));
        semaforos[0].nombre = S;

    }
    printf("[debug] ID en la estructura: %d\n", semaforos[0].id);
    printf("[debug] TIPO en la estructura: %c\n", semaforos[0].tipo);
    printf("[debug] NOMBRE en la estructura: %s\n", semaforos[0].nombre);
    //si no hay ningún semáforo asignado, asignar el primero
    // si hay algún semáforo asignado:
    // comprobar si el siguiente está disponible
    // comprobar que no ha sido asignado ya
    // asignar o pasar al siguiente si ya estaba asignado o no estaba disponible



}