// Ejecuta el ciclo principal de P1; afecta stdout user-space; depende de PRINT, p1_delay, p1_fault_demo y sys_yield.
#include "p1.h"
#include "../lib/print.h"
#include "../lib/user_syscalls.h"

void p1_run(void) {
    uint32_t p1_n = 0u;

    for (;;) {
        PRINT("P1:%d\n", (int)p1_n);
        p1_n = (p1_n + 1u) % 10u;
        p1_delay();
        p1_fault_demo(p1_n);
        sys_yield();
    }
}
