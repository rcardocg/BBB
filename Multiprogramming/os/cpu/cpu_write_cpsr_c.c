#include "cpu.h"
/* Escribe en los bits de control del CPSR. Afecta: CPSR_c. Deps: Ninguna. */
void cpu_write_cpsr_c(uint32_t v) {
    __asm__ volatile ("msr cpsr_c, %0" :: "r"(v) : "cc", "memory");
}
