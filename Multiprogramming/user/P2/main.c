#include "../lib/stdio.h"

void main() {
    char c = 'a';

    while (1) {
        PRINT("----From P1: %c\n", c);
        c = (c == 'z') ? 'a' : c + 1;

        /* Short delay so output is readable between timer ticks */
        //for (volatile int i = 0; i < 1000000; i++);
    }
}