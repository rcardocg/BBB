#include <stdint.h>

#include "../lib/print.h"

// Definición de la syscall yield
extern void __sys_yield(void);

int main(void) {
    char c = 'a';

    for (;;) {
        PRINT("P2:%c\n", c);
        c = (c == 'z') ? 'a' : (char)(c + 1);
        __sys_yield();
    }
}
