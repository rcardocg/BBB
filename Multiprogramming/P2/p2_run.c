// Ejecuta el ciclo principal de P2; afecta stdout user-space; depende de PRINT, p2_delay y sys_yield.
#include "p2.h"
#include "../lib/print.h"
#include "../lib/user_syscalls.h"

void p2_run(void) {
    char p2_c = 'a';

    for (;;) {
        PRINT("P2:%c\n", p2_c);
        p2_c = (p2_c == 'z') ? 'a' : (char)(p2_c + 1);
        p2_delay();
        sys_yield();
    }
}
