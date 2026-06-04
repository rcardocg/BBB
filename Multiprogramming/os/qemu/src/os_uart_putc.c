#include "os_qemu.h"
#define UART0_BASE 0x101f1000u
#define UART_DR 0x00u
#define UART_FR 0x18u
#define UART_FR_TXFF (1u << 5)
/* Escribe un caracter en UART. Afecta: UART0. Deps: Ninguna. */
void os_uart_putc(char c) {
    while ((*(volatile uint32_t *)(UART0_BASE + UART_FR)) & UART_FR_TXFF);
    (*(volatile uint32_t *)(UART0_BASE + UART_DR)) = (uint32_t)c;
}
