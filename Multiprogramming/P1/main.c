#include <stdint.h>

#include "../lib/print.h"

// Definición de la syscall yield
extern void __sys_yield(void);

int main(void) {
    uint32_t n = 0;

    for (;;) {
        PRINT("P1:%d\n", (int)n);
        n = (n + 1u) % 10u;
        __sys_yield();
    }
}
