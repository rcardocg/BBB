// Ejecuta un retraso visible para P1; afecta ciclos de CPU user-space; no tiene dependencias.
#include "p1.h"

void p1_delay(void) {
    volatile uint32_t p1_delay_i;

    for (p1_delay_i = 0u; p1_delay_i < 10000000u; p1_delay_i++) {
        __asm__ volatile("nop");
    }
}
