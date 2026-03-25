#ifndef PRINT_H
#define PRINT_H

#include <stdint.h>

void uart_putc(char c);
void uart_puts(const char *s);
void PRINT(const char *fmt, ...);

#endif
