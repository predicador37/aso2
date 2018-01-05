//
// Created by Miguel Expósito Martín, 72056097H.
//

#ifndef ASO2_LIBSEM_H
#define ASO2_LIBSEM_H

// estructura donde se almacenará la información de cada semáforo
struct semaforo {
    int id;
    char tipo;
    char *nombre;
};
typedef struct semaforo Semaforo;

// se crea un puntero a estructuras de tipo semáforo que podrá utilizarse en otros ficheros
extern Semaforo* semaforos;

/* a_sem
 *
 * Parámetros de entrada:
 *
 * char *S: nombre del semáforo
 * char tipo: tipo del semáforo
 * int N: número de elementos del conjunto de semáforos
 *
 * Descripción:
 *
 * Si es la primera vez que se invoca, crea un conjunto de semáforos y asocia al primero el nombre especificado en S
 * y el tipo especificado en tipo. Si la función se vuelve a invocar, asocia a otro semáforo del conjunto el nombre y
 * tipo especificados como argumentos de entrada. Antes de realizar la asignación comprueba que existen semáforos
 * disponibles que no tienen un nombre asignado y comprueba que el tipo corresponde a uno de los dos tipos permitidos.
 *
 */
void a_sem(char S[], char tipo, int N);

/* i_sem
 *
 * Parámetros de entrada:
 *
 * char *S: nombre del semáforo
 * int x: valor con el que se inicializa el semáforo
 *
 * Descripción:
 *
 * Esta función inicializa el semáforo de nombre S con el valor x. Antes de realizar la inicialización comprueba
 * que x cumple con las especificaciones asociadas al tipo de semáforo.
 *
 */
void i_sem(char S[], int x);

/* w_sem
 *
 * Parámetros de entrada:
 *
 * char *S: nombre del semáforo
 *
 *
 * Descripción:
 *
 * Esta función  implementa la operación wait_sem sobre el semáforo de nombre S.
 *
 */
void w_sem(char S[]);

/* s_sem
 *
 * Parámetros de entrada:
 *
 * char *S: nombre del semáforo
 *
 *
 * Descripción:
 *
 * Esta función implementa la operación signal_sem sobre el semáforo de nombre S.
 * Debe tenerse en cuenta que si se trata de un semaforo binario cuyo valor ya se encuentra
 * a 1 esta operación no produce ningún efecto
 *
 */
void s_sem(char S[]);

/* z_sem
 *
 * Parámetros de entrada:
 *
 * No recibe ningún parámetro
 *
 * Descripción:
 *
 * Esta función elimina el conjunto de semaforos que fue creado en la primera invocación de la función a_sem
 *
 */
void z_sem();

/*
 * siguiente_sin_asignar
 *
 * Función auxiliar que determina cuál es el siguiente semáforo sin asignar.
 * Para ello, devuelve el índice del primer semáforo sin nombre asignado en la estructura.
 *
 */
int siguiente_sin_asignar();

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
int busca_semaforo(char S[]);

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
int condicion_binario(int x);

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
int condicion_general(int x);

#endif //ASO2_LIBSEM_H
