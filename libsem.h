//
// Created by predicador on 9/12/17.
//

#ifndef ASO2_LIBSEM_H
#define ASO2_LIBSEM_H

struct semaforo {
    int id;
    char tipo;
    char *nombre;
};
typedef struct semaforo Semaforo;

extern Semaforo* semaforos;

void a_sem(char S[], char tipo, int N);

#endif //ASO2_LIBSEM_H
