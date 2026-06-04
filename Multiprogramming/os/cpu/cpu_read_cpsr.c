#include "cpu.h"
/* Lee el registro de estado actual (CPSR). Afecta: R0. Deps: Ninguna. */
uint32_t cpu_read_cpsr(void) {
    uint32_t v;
    __asm__ volatile ("mrs %0, cpsr" : "=r"(v));
    return v;
}
