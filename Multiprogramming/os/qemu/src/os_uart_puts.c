#include "os_qemu.h"
/* Escribe una cadena en UART. Afecta: UART0. Deps: os_uart_putc. */
void os_uart_puts(const char *s) {
    while (*s != '\0') {
        if (*s == '\n') os_uart_putc('\r');
        os_uart_putc(*s++);
    }
}
