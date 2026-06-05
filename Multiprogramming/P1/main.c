#include <stdint.h>

#include "../lib/print.h"
#include "../lib/user_syscalls.h"

/* MODO DE DEMOSTRACIÓN
 * 0 para "Ejecución Normal".
 * 1 para "Contención de Fallas".
 */
#define FORCE_FAULT 1

int main(void) {
    uint32_t n = 0;
    volatile uint32_t delay;

    for (;;) {
        PRINT("P1:%d\n", (int)n);
        n = (n + 1u) % 10u;
        
        /* Retraso de software para ver los logs despacio */
        // for (delay = 0; delay < 10000000; delay++) {
        //     __asm__ volatile("nop");
        // }

#if FORCE_FAULT
        if (n == 4u) {
            /* Forzar un fallo. Como QEMU no tiene la MMU activada en este proyecto, 
               los errores de memoria (Data Abort) no saltan al escribir en direcciones 
               inválidas como 0xDEADBEEF. 
               Para probar que el kernel aísla fallos, ejecutaremos una instrucción 
               indefinida (Undefined Instruction) que sí dispara una excepción de hardware. */
            PRINT("P1: Intentando ejecutar instruccion ilegal...\n");
            __asm__ volatile(".word 0xE7F000F0"); /* Instrucción indefinida en ARM */
        }
#endif

        sys_yield();
    }
}
