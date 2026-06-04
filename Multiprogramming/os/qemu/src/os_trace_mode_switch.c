#include "os_qemu.h"
/* Trace obligatorio del Inciso 3.4. Afecta: UART0. Deps: os_uart_puts. */
void os_trace_mode_switch(uint32_t pid, const char *reason) {
    os_uart_puts("\nMODE_SWITCH KERNEL_TO_USER pid=");
    os_uart_putc((char)(pid + '0'));
    os_uart_puts(" reason=");
    os_uart_puts(reason);
    os_uart_puts("\n");
}
