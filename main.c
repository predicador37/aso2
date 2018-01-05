//
// Created by Miguel Expósito Martín, 72056097H.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include "libsem.h"

int n_inicial; //número de procesos a crear sin modificar por la función recursiva
extern Semaforo* semaforos;

int crea_procesos_encadenados(int n, int N)
{
    printf("Crea proceso con n = %d\n", n_inicial - n);
    if (n == 0) // no hay más procesos por crear
    {
        printf("NO HAY MÁS PROCESOS POR CREAR\n");
        s_sem(semaforos[0].nombre); //devolver control a patriarca
        return 0;
    }


    int pid = fork();

    if (pid == -1) {
        perror("[error] No se pudo crear el proceso hijo.\n");
    }
    if (pid == 0) { // proceso hijo
        n--;
        //printf("[child] Wait for smaphore: %d\n", n_inicial-n);
        printf("Creando proceso %d\n", n_inicial - n);
        crea_procesos_encadenados(n, N);
        exit(0);
    }
    else { // proceso padre...

        printf("[parent] WAIT for smaphore: %s\n", semaforos[n_inicial - n].nombre);
        printf("CONTROL_WAIT_3\n");
        w_sem(semaforos[n_inicial - n ].nombre); // esperar para retomar el control
        int i =0;
        for(i=0;i<N;i++) {
            printf("CONTROL DEVUELTO A %d\n", getpid());
            printf("Proceso %d, PID = %d\n", n_inicial - n, getpid());
            sleep(1);
            if (n!= 1) {
                s_sem(semaforos[n_inicial - n + 1].nombre); // activar semáforo
                printf("CONTROL_WAIT_1\n");
                w_sem(semaforos[n_inicial - n].nombre); // esperar para retomar el control// de hijo
            }
            else {
                s_sem(semaforos[0].nombre);
                printf("CONTROL_WAIT_2\n");
                w_sem(semaforos[n_inicial - n].nombre); // esperar para retomar el control// de hijo
            }


        }
        if (n!=1) {
            s_sem(semaforos[n_inicial - n + 1].nombre);
            exit(0);

        }
        else {
            z_sem();
            exit(0);
        }

    }


}

int main(int argc, char **argv) {

    if (argc != 3) {
        perror("[error] Debe introducir exactamente dos argumentos: número de procesos y de sincronizaciones.");
        return(1);
    }

    int n = atoi(argv[1]);
    int N = atoi(argv[2]);

    if (n < 2 || n > 8) {
        perror("[error] El número de procesos a crear debe estar entre 2 y 8.");
        return(1);
    }

    if (N < 2 || N > 15) {
        perror("[error] El número de veces a ejecutar la secuencia de sincronización debe estar entre 2 y 15.");
        return(1);
    }
    n_inicial = n;

    /* A continuación se generan los nombres para los semáforos: MUTEX1, MUTEX2, ...*/

    char *nombre_base = "MUTEX";
    int len_nombre = strlen(nombre_base);
    int i=0;
    for (i=0;i<n;i++) {
        char numero = i + '0';
        char *nombre = malloc(len_nombre + 1 + 1); //fin de cadena + numero (menor que 8) + caracter final
        strcpy(nombre, nombre_base);
        nombre[len_nombre] = numero;
        nombre[len_nombre + 1] = '\0';
        printf("NOMBRE: %s\n", nombre);
        a_sem(nombre, 'B', n);
        i_sem(nombre, 0);
    }


    //printf("Nombre del semáforo: %s\n", semaforos[0].nombre);
    crea_procesos_encadenados(n, N);
    exit(0);
}