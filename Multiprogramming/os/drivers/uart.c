#include "uart.h"
#include "mmio.h"

#define UART0_BASE      0x44E09000u
#define UART_THR        0x00u
#define UART_LSR        0x14u
#define UART_LSR_THRE   (1u << 5)

void uart_putc(char c) {
    while ((mmio_read(UART0_BASE + UART_LSR) & UART_LSR_THRE) == 0u) {
    }
    mmio_write(UART0_BASE + UART_THR, (uint32_t)c);
}

void uart_puts(const char *s) {
    while (*s != '\0') {
        uart_putc(*s++);
    }
}
