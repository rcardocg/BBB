#include "os_qemu.h"
/* Despacha syscalls. Afecta: UART0, CPU State. Deps: os_uart_putc, os_timer_irq_handler. */
void os_syscall_dispatcher(uint32_t *frame) {
    uint32_t syscall_num = frame[7];
    if (syscall_num == 1) os_uart_putc((char)frame[0]);
    else if (syscall_num == 2) os_timer_irq_handler(frame);
    else os_uart_puts("\n[KERNEL] Unknown syscall\n");
}
