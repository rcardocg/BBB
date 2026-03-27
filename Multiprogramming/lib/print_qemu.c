#include "print.h"
#include <stdarg.h>

typedef unsigned int u32;

#define MMIO32(addr) (*(volatile u32 *)(addr))

// UART0 en VersatilePB (PL011)
#define UART0_BASE    0x101f1000

#define UART_DR       0x00    // Data Register
#define UART_FR       0x18    // Flag Register

#define UART_FR_TXFF  (1 << 5)  // Transmit FIFO Full

static inline u32 mmio_read(u32 addr) {
    return MMIO32(addr);
}

static inline void mmio_write(u32 addr, u32 value) {
    MMIO32(addr) = value;
}

void uart_putc(char c) {
    // newline fix
    if (c == '\n') {
        uart_putc('\r');
    }

    // esperar mientras FIFO está lleno
    while (mmio_read(UART0_BASE + UART_FR) & UART_FR_TXFF) {
    }

    mmio_write(UART0_BASE + UART_DR, (u32)c);
}

void uart_puts(const char *s) {
    while (*s) {
        uart_putc(*s++);
    }
}

static void uart_put_u32_dec(u32 value) {
    char buf[10];
    u32 i = 0;

    if (value == 0) {
        uart_putc('0');
        return;
    }

    while (value > 0 && i < sizeof(buf)) {
        buf[i++] = '0' + (value % 10);
        value /= 10;
    }

    while (i > 0) {
        uart_putc(buf[--i]);
    }
}

void PRINT(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);

    const char *p = fmt;

    while (*p) {
        if (*p != '%') {
            uart_putc(*p++);
            continue;
        }

        p++;

        if (*p == '%') {
            uart_putc('%');
            p++;
            continue;
        }

        if (*p == 'd') {
            int v = va_arg(ap, int);
            uart_put_u32_dec((u32)v);
            p++;
            continue;
        }

        if (*p == 'c') {
            char c = (char)va_arg(ap, int);
            uart_putc(c);
            p++;
            continue;
        }

        if (*p == 's') {
            const char *s = va_arg(ap, const char *);
            uart_puts(s ? s : "(null)");
            p++;
            continue;
        }

        uart_putc('%');
        uart_putc(*p++);
    }

    va_end(ap);
}