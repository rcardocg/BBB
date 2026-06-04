#include "os_qemu.h"
/* Trace de fallos del Inciso 3.6. Afecta: UART0. Deps: os_uart_puts, os_uart_putc. */
void os_trace_fault(uint32_t pid, const char *type) {
    os_uart_puts("\nMODE_SWITCH USER_TO_KERNEL pid=");
    os_uart_putc((char)(pid + '0'));
    os_uart_puts(" reason=fault type=");
    os_uart_puts(type);
    os_uart_puts("\n");
}
