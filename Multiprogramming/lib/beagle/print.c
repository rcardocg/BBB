#include "print.h"

#include <stdarg.h>
#include <stdint.h>

typedef uint32_t u32;

#define PRINT_BUF_SIZE 96u

extern int32_t __sys_write(int32_t fd, const void *buf, u32 len);

static void buf_putc(char *buf, u32 *len, char c) {
    if (*len < PRINT_BUF_SIZE) {
        buf[*len] = c;
        *len = *len + 1u;
    }
}

static void buf_puts(char *buf, u32 *len, const char *s) {
    while (*s != '\0') {
        if (*s == '\n') {
            buf_putc(buf, len, '\r');
        }
        buf_putc(buf, len, *s++);
    }
}

static void buf_put_u32_dec(char *out, u32 *len, u32 value) {
    char digits[10];
    u32 i = 0;

    if (value == 0u) {
        buf_putc(out, len, '0');
        return;
    }

    while (value > 0u && i < (u32)sizeof(digits)) {
        digits[i++] = (char)('0' + (value % 10u));
        value /= 10u;
    }

    while (i > 0u) {
        buf_putc(out, len, digits[--i]);
    }
}

void uart_putc(char c) {
    char out[2];
    u32 len = 0u;

    if (c == '\n') {
        buf_putc(out, &len, '\r');
    }
    buf_putc(out, &len, c);
    (void)__sys_write(1, out, len);
}

void uart_puts(const char *s) {
    char out[PRINT_BUF_SIZE];
    u32 len = 0u;

    buf_puts(out, &len, s);
    (void)__sys_write(1, out, len);
}

void PRINT(const char *fmt, ...) {
    va_list ap;
    char out[PRINT_BUF_SIZE];
    u32 len = 0u;
    const char *p = fmt;

    va_start(ap, fmt);

    while (*p != '\0') {
        if (*p != '%') {
            if (*p == '\n') {
                buf_putc(out, &len, '\r');
            }
            buf_putc(out, &len, *p++);
            continue;
        }

        p++;
        if (*p == '\0') {
            break;
        }

        if (*p == '%') {
            buf_putc(out, &len, '%');
            p++;
            continue;
        }

        if (*p == 'd') {
            u32 v = (u32)va_arg(ap, int);
            buf_put_u32_dec(out, &len, v);
            p++;
            continue;
        }

        if (*p == 'c') {
            char c = (char)va_arg(ap, int);
            if (c == '\n') {
                buf_putc(out, &len, '\r');
            }
            buf_putc(out, &len, c);
            p++;
            continue;
        }

        if (*p == 's') {
            const char *s = va_arg(ap, const char *);
            buf_puts(out, &len, s ? s : "(null)");
            p++;
            continue;
        }

        buf_putc(out, &len, '%');
        buf_putc(out, &len, *p++);
    }

    va_end(ap);

    if (len > 0u) {
        (void)__sys_write(1, out, len);
    }
}
