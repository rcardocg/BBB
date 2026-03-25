#include <stdint.h>

#include "../lib/print.h"

int main(void) {
    char c = 'a';

    for (;;) {
        PRINT("P2:%c\n", c);
        c = (c == 'z') ? 'a' : (char)(c + 1);
        __asm__ volatile ("wfi");
    }
}
