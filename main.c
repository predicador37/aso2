#include <stdio.h>
#include "libsem.h"

int main() {
    printf("Hello, World!\n");
    a_sem("SEMAFORO", 'B', 3);
    return 0;
}