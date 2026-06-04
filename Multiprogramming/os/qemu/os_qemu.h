#ifndef OS_QEMU_H
#define OS_QEMU_H

#include <stdint.h>

/* Escribe un caracter en UART. */
void os_uart_putc(char c);

/* Escribe una cadena en UART. */
void os_uart_puts(const char *s);

/* Deshabilita el Watchdog Timer. */
void os_wdt_disable(void);

/* Inicializa la tabla de procesos. */
void os_pcb_system_init(void);

/* Despacha las llamadas al sistema. */
void os_syscall_dispatcher(uint32_t *frame);

/* Maneja los fallos del procesador. */
void os_fault_handler(uint32_t *frame);

/* Maneja la interrupción del timer. */
void os_timer_irq_handler(uint32_t *irq_frame);

/* Imprime rastro de cambio de modo (direccion: USER_TO_KERNEL o KERNEL_TO_USER). */
void os_trace_mode_switch(uint32_t pid, const char *direction, const char *reason, const char *extra);

/* Imprime rastro de fallo (USER_TO_KERNEL fault type=...). */
void os_trace_fault(uint32_t pid, const char *type);

/* Maneja SYS_YIELD con cambio de contexto. */
void os_handle_sys_yield(uint32_t *frame);

/* Punto de entrada principal del Kernel. */
void os_kmain(void);

#endif
