// Dispara la demo de fallo de P1 cuando esta habilitada; afecta excepciones ARM; depende de PRINT.
#include "p1.h"
#include "../lib/print.h"

void p1_fault_demo(uint32_t n) {
#if P1_FORCE_FAULT
    if (n == 4u) {
        PRINT("P1: Intentando ejecutar instruccion ilegal...\n");
        __asm__ volatile(".word 0xE7F000F0");
    }
#else
    (void)n;
#endif
}
