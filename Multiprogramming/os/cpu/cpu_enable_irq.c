#include "cpu.h"
/* Habilita las interrupciones IRQ globalmente. Afecta: CPSR_i. Deps: Ninguna. */
void cpu_enable_irq(void) {
    __asm__ volatile ("cpsie i" ::: "memory");
}
