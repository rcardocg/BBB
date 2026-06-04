#include "os_qemu.h"
/* Trace unificado de cambios de modo (Inciso 3.8). Afecta: UART0. Deps: os_uart_puts. */
void os_trace_mode_switch(uint32_t pid, const char *direction, const char *reason, const char *extra) {
    os_uart_puts("\nMODE_SWITCH ");
    os_uart_puts(direction);
    os_uart_puts(" pid=");
    os_uart_putc((char)(pid + '0'));
    os_uart_puts(" reason=");
    os_uart_puts(reason);
    if (extra != 0) {
        os_uart_puts(" ");
        os_uart_puts(extra);
    }
    os_uart_puts("\n");
}
