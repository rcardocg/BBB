#include "print.h"

#include <stdarg.h>

typedef uint32_t u32;

#define MMIO32(addr) (*(volatile u32 *)(addr))

#define UART0_BASE    0x44E09000u
#define UART_THR      0x00u
#define UART_LSR      0x14u
#define UART_LSR_THRE (1u << 5)

static inline u32 mmio_read(u32 addr) { return MMIO32(addr); }
static inline void mmio_write(u32 addr, u32 value) { MMIO32(addr) = value; }

void uart_putc(char c) {
    if (c == '\n') {
        uart_putc('\r');
    }
    while ((mmio_read(UART0_BASE + UART_LSR) & UART_LSR_THRE) == 0u) {
    }
    mmio_write(UART0_BASE + UART_THR, (u32)c);
}

void uart_puts(const char *s) {
    while (*s != '\0') {
        uart_putc(*s++);
    }
}

static void uart_put_u32_dec(u32 value) {
    char buf[10];
    u32 i = 0;

    if (value == 0u) {
        uart_putc('0');
        return;
    }

    while (value > 0u && i < (u32)sizeof(buf)) {
        buf[i++] = (char)('0' + (value % 10u));
        value /= 10u;
    }

    while (i > 0u) {
        uart_putc(buf[--i]);
    }
}

void PRINT(const char *fmt, ...) {
    va_list ap;
    const char *p = fmt;

    va_start(ap, fmt);

    while (*p != '\0') {
        if (*p != '%') {
            uart_putc(*p++);
            continue;
        }

        p++;
        if (*p == '\0') {
            break;
        }

        if (*p == '%') {
            uart_putc('%');
            p++;
            continue;
        }

        if (*p == 'd') {
            u32 v = (u32)va_arg(ap, int);
            uart_put_u32_dec(v);
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
            if (s == 0) {
                uart_puts("(null)");
            } else {
                uart_puts(s);
            }
            p++;
            continue;
        }

        uart_putc('%');
        uart_putc(*p++);
    }

    va_end(ap);
}
