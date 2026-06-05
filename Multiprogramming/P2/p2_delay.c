// Ejecuta un retraso visible para P2; afecta ciclos de CPU user-space; no tiene dependencias.
#include "p2.h"

void p2_delay(void) {
    volatile uint32_t p2_delay_i;

    for (p2_delay_i = 0u; p2_delay_i < 10000000u; p2_delay_i++) {
        __asm__ volatile("nop");
    }
}
