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
extern Semaforo* semaforos; //se importa la estructura de libsem

/* crea_procesos_encadenados
 *
 * Parámetros de entrada:
 *
 * int n: número de procesos a crear
 * int N: número de veces a ejecutar la secuencia de sincronización
 *
 *
 * Descripción:
 *
 * Esta función recursiva genera la cadena de procesos requerida usando fork y los sincroniza utilizando semáforos
 * binarios.
 *
 */
int crea_procesos_encadenados(int n, int N)
{

    if (n == 0) // no hay más procesos por crear
    {
        s_sem(semaforos[0].nombre); //devolver control a patriarca
        return 0;
    }

    // se crea un proceso hijo
    int pid = fork();

    if (pid == -1) {
        perror("[error] No se pudo crear el proceso hijo.\n");
    }
    if (pid == 0) { // proceso hijo
        n--; // decrementar el contador de procesos, ya que hay uno más
        crea_procesos_encadenados(n, N); // llamar recursivamente a la propia función
    }
    else { // proceso padre...

        w_sem(semaforos[n_inicial - n ].nombre); // esperar para retomar el control
        int i =0;
        // secuencia de sincronización
        for(i=0;i<N;i++) {
            printf("Proceso %d, PID = %d\n", n_inicial - n + 1, getpid());
            sleep(1); //esperar un segundo
            // si el proceso no es el último, activa a su hijo y queda a la espera
            if (n!= 1) {
                s_sem(semaforos[n_inicial - n + 1].nombre); // activar semáforo del hijo
                w_sem(semaforos[n_inicial - n].nombre); // esperar para retomar el control de hijo
            }
            else { //n=1, último proceso
                printf("\n");
                s_sem(semaforos[0].nombre); // devuelve el control al patriarca
                w_sem(semaforos[n_inicial - n].nombre); // esperar para retomar el control de hijo

            }


        }
        // cierre de todos los procesos excepto el último
        if (n!=1) {
            s_sem(semaforos[n_inicial - n + 1].nombre);
            exit(0);

        }
        else { //n=1, último proceso
            //eliminar el conjunto de semáforos y salir
            z_sem();
            exit(0);
        }

    }

}

/*
 * Programa principal que recibirá los argumentos de línea de comandos y creará el conjunto de semáforos necesario
 * para después invocar a la función que creará los procesos y mostrará la secuencia de sincronización
 *
 */
int main(int argc, char **argv) {

    // comprobación del número de argumentos
    if (argc != 3) {
        perror("[error] Debe introducir exactamente dos argumentos: número de procesos y de sincronizaciones.");
        exit(-1);
    }

    int n = atoi(argv[1]);
    int N = atoi(argv[2]);

    // comprobación del argumento de número de procesos
    if (n < 2 || n > 8) {
        perror("[error] El número de procesos a crear debe estar entre 2 y 8.");
        exit(-1);
    }

    // comprobación del argumento de número de veces a ejecutar la secuencia de sincronización
    if (N < 2 || N > 15) {
        perror("[error] El número de veces a ejecutar la secuencia de sincronización debe estar entre 2 y 15.");
        exit(-1);
    }
    n_inicial = n;

    /* A continuación se generan los nombres para los semáforos: SEM1, SEM2, ...*/
    char *nombre_base = "SEM";
    int len_nombre = strlen(nombre_base);
    int i=0;
    for (i=0;i<n;i++) {
        char numero = i + '0';
        char *nombre = malloc(len_nombre + 1 + 1); //fin de cadena + numero (menor que 8) + caracter final
        strcpy(nombre, nombre_base);
        nombre[len_nombre] = numero;
        nombre[len_nombre + 1] = '\0';
        // se crean los semáforos (la primera vez también en conjunto) con los nombres generados y se inicializan a 0
        // (un semáforo binario por proceso)
        a_sem(nombre, 'B', n);
        i_sem(nombre, 0);
    }


    //se crean los procesos encadenados que usarán los semáforos creados anteriormente
    //la llamada a esta función mostrará la secuencia de sincronización por pantalla
    crea_procesos_encadenados(n, N);
    exit(0);
}