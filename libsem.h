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
void i_sem(char S[], int x);
void w_sem(char S[]);
void s_sem(char S[]);
void z_sem();

int obtener_valor_semaforo(int sem);
int siguiente_sin_asignar();
int busca_semaforo(char S[]);
int condicion_binario(int x);
int condicion_general(int x);

#endif //ASO2_LIBSEM_H
