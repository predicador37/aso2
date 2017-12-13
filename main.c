#include <stdio.h>
#include "libsem.h"

int main() {
    printf("Hello, World!\n");
    a_sem("SEMAFORO1", 'B', 3);
    a_sem("SEMAFORO2", 'G', 3);
    a_sem("SEMAFORO3", 'B', 3);
    i_sem("SEMAFORO2", 5);


    return 0;
}