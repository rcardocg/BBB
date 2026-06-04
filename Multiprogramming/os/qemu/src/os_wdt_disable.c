#include "os_qemu.h"

/* Deshabilita el WDT. Afecta: WDT1. Deps: Ninguna. */
void os_wdt_disable(void) {
    os_uart_puts("[KERNEL] WDT skip (QEMU)\n");
}
