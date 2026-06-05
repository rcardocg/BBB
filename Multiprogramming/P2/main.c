#include <stdint.h>

#include "../lib/print.h"
#include "../lib/user_syscalls.h"

int main(void) {
    char c = 'a';
    volatile uint32_t delay;

    for (;;) {
        PRINT("P2:%c\n", c);
        c = (c == 'z') ? 'a' : (char)(c + 1);

        /* Retraso de software para ver los logs despacio */
        for (delay = 0; delay < 10000000; delay++) {
            __asm__ volatile("nop");
        }

        sys_yield();
    }
}
