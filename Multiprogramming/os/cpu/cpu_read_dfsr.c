#include "cpu.h"
/* Lee el Data Fault Status Register. Afecta: R0. Deps: Ninguna. */
uint32_t cpu_read_dfsr(void) {
    uint32_t v;
    __asm__ volatile ("mrc p15, 0, %0, c5, c0, 0" : "=r"(v));
    return v;
}
