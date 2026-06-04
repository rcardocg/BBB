#include "cpu.h"
/* Lee el Instruction Fault Status Register. Afecta: R0. Deps: Ninguna. */
uint32_t cpu_read_ifsr(void) {
    uint32_t v;
    __asm__ volatile ("mrc p15, 0, %0, c5, c0, 1" : "=r"(v));
    return v;
}
